#include "connection-widget.h"
#include "ui_connection-widget.h"

#include <glrt/toolkit/network.h>

const quint64 MAX_TIME = std::numeric_limits<quint64>::max();

ConnectionWidget::ConnectionWidget(QTcpSocket* tcpSocket, QWidget *parent) :
  QWidget(parent),
  applicationName("Connected Game"),
  ui(new Ui::ConnectionWidget),
  tcpSocket(tcpSocket)
{
  ui->setupUi(this);

  ui->treeView->setModel(this->createModel());
  ui->treeView->setColumnWidth(COLUMN_NAME, (ui->treeView->columnWidth(COLUMN_NAME)*6)/4);
  ui->treeView->setColumnWidth(COLUMN_CPU_TIME, ui->treeView->columnWidth(COLUMN_CPU_TIME)/2);
  ui->treeView->setColumnWidth(COLUMN_GPU_TIME, ui->treeView->columnWidth(COLUMN_GPU_TIME)/2);
  ui->treeView->setColumnWidth(COLUMN_FUNCTION, (ui->treeView->columnWidth(COLUMN_FUNCTION)*5)/2);
  ui->treeView->setColumnWidth(COLUMN_LINE, ui->treeView->columnWidth(COLUMN_LINE)/2);
  ui->treeView->setColumnWidth(COLUMN_FILE, ui->treeView->columnWidth(COLUMN_FILE)*2);

  connect(tcpSocket, SIGNAL(aboutToClose()), this, SLOT(deleteLater()));
  connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
  connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));

  requestString(0);
  tcpSocket->flush();
}

ConnectionWidget::~ConnectionWidget()
{
  tcpSocket->abort();

  delete ui;
  delete tcpSocket;
}

void ConnectionWidget::dataReceived()
{
  sendModelStartChangeSignal();

  bool hasData = false;
#if 1 // Use the newsest frame
  qint64 bytes = tcpSocket->bytesAvailable();
  do
  {
    QBuffer networkBuffer;
    networkBuffer.open(QIODevice::ReadOnly);
    if(glrt::Network::readAtomic(&networkBuffer.buffer(), *tcpSocket, 0, 0))
    {
      bytes -= networkBuffer.size();
      hasData = true;
      handleData(&networkBuffer);
    }else
    {
      break;
    }
  }while(tcpSocket->bytesAvailable() > 0 && bytes > 0 && hasData);

  if(hasData)
    updateGui();
#else // Don't skip any data
  QBuffer networkBuffer;
  networkBuffer.open(QIODevice::ReadOnly);
  hasData = glrt::Network::readAtomic(&networkBuffer.buffer(), *tcpSocket, 0, 0);
  if(hasData)
  {
    handleData(&networkBuffer);
    updateGui();
  }
#endif

  sendModelChangedEndSignal();
}

void ConnectionWidget::handleData(QBuffer* networkBuffer)
{
  int n;
  QDataStream stream(networkBuffer);

  stream >> n;
  for(int i=0; i<n; ++i)
  {
    quintptr ptr;
    stream >> ptr;
    stream >> strings[ptr];

    if(ptr == 0 && applicationName!=strings[ptr])
    {
      applicationName = strings[ptr];
      applicationNameChanged(applicationName);
    }
  }

  stream >> n;
  currentData.resize(n);
  for(int i=0; i<n; ++i)
  {
    DataLine& dataLine = currentData[i];
    quint64& cpuTime = dataLine.cpuTime;
    quint64& gpuTime = dataLine.gpuTime;
    quintptr& file =  dataLine.file;
    quintptr& function = dataLine.function;
    quintptr& name = dataLine.name;
    int& line = dataLine.line;
    int& depth = dataLine.depth;

    stream >> cpuTime >> gpuTime >> file >> function >> name >> line >> depth;

    requestString(file);
    requestString(function);
    requestString(name);
  }
  tcpSocket->flush();

  stream >> this->frameTime;
}

void ConnectionWidget::updateGui()
{
  ui->labelFPS->setText(QString("FPS: %0").arg(1.f/this->frameTime));
}

void ConnectionWidget::requestString(quintptr ptr)
{
  if(!strings.contains(ptr))
    tcpSocket->write(reinterpret_cast<char*>(&ptr), sizeof(quintptr));
}

class ConnectionWidget::DataModel : public QAbstractItemModel
{
public:
  QVector<DataLine>& currentData;
  const QHash<quintptr, QString>& strings;
  ConnectionWidget* widget;

  DataModel(ConnectionWidget* w)
    : QAbstractItemModel(w),
      currentData(w->currentData),
      strings(w->strings),
      widget(w)
  {
  }

  DataLine* firstChildOf(const QModelIndex& parent) const
  {
    if(currentData.isEmpty())
      return nullptr;

    if(parent.isValid())
    {
      DataLine* firstRow = const_cast<DataLine*>(reinterpret_cast<const DataLine*>(parent.internalPointer()));
      int expectedDepth = firstRow->depth+1;

      DataLine* childRow = firstRow+1;
      if(childRow->depth == expectedDepth)
        return childRow;
    }else
    {
      Q_ASSERT(currentData.data()->depth == 0);
      return currentData.data();
    }

    return nullptr;
  }

  QModelIndex index(int row, int column, const QModelIndex& parent) const override
  {
    if(column >= N_COLUMNS)
      return QModelIndex();

    DataLine* lastRow = &currentData.last();


    DataLine* childRow = firstChildOf(parent);
    if(childRow == nullptr)
      return QModelIndex();

    int r = 0;
    int expectedDepth = childRow->depth;

    while(childRow<=lastRow && childRow->depth>=expectedDepth)
    {
      if(childRow->depth == expectedDepth)
      {
        if(r == row)
          return createIndex(row, column, childRow);
        r++;
      }
      childRow++;
    }

    return QModelIndex();
  }

  QModelIndex parent(const QModelIndex& child) const override
  {
    if(!child.isValid())
      return QModelIndex();

    DataLine* childRow = const_cast<DataLine*>(reinterpret_cast<const DataLine*>(child.internalPointer()));

    DataLine* firstRow = currentData.data();

    if(firstRow == childRow)
      return QModelIndex();

    DataLine* row = childRow-1;

    while(row >= firstRow && row->depth >= childRow->depth)
    {
      row--;
    }

    if(row < firstRow)
      return QModelIndex();

    DataLine* parentRow = row;
    int rowIndex = 0;
    row--;

    while(row >= firstRow && row->depth >= parentRow->depth)
    {
      if(row->depth == parentRow->depth)
        rowIndex++;
      row--;
    }

    return createIndex(rowIndex, child.column(), parentRow);
  }

  int rowCount(const QModelIndex& parent) const override
  {
    DataLine* dataLine = firstChildOf(parent);

    if(dataLine == nullptr)
      return 0;

    DataLine* lastRow = &currentData.last();
    int expectedDepth = dataLine->depth;
    int nRows = 0;

    while(lastRow>=dataLine && dataLine->depth>=expectedDepth)
    {
      if(dataLine->depth == expectedDepth)
        nRows++;
      dataLine++;
    }

    return nRows;
  }

  int columnCount(const QModelIndex&) const override
  {
    return N_COLUMNS;
  }

  QVariant data(const QModelIndex& index, int role) const override
  {
    if(!index.isValid())
      return QVariant();

    const DataLine& data = *reinterpret_cast<const DataLine*>(index.internalPointer());

    if(role == Qt::DisplayRole)
    {
      switch(index.column())
      {
      case COLUMN_CPU_TIME:
        return data.cpuTime!=MAX_TIME ? QVariant(data.cpuTime) : QVariant(QString("-"));
      case COLUMN_GPU_TIME:
        return data.gpuTime!=MAX_TIME ? QVariant(data.gpuTime) : QVariant(QString("-"));
      case COLUMN_FILE:
        return QVariant(QFileInfo(strings.value(data.file, "???")).fileName());
      case COLUMN_FUNCTION:
        return QVariant(strings.value(data.function, "???"));
      case COLUMN_NAME:
        return QVariant(strings.value(data.name, "???"));
      case COLUMN_LINE:
        return QVariant(data.line);
      default:
        return QVariant();
      }
    }else if(role == Qt::ToolTipRole)
    {
      switch(index.column())
      {
      case COLUMN_FILE:
        return QVariant(QFileInfo(strings.value(data.file, "???")).absoluteFilePath());
      default:
        return QVariant();
      }
    }

    return QVariant();
  }

  QVariant headerData(int section, Qt::Orientation, int role) const override
  {
    if(role != Qt::DisplayRole)
      return QVariant();
    switch(section)
    {
    case COLUMN_CPU_TIME:
      return QVariant(QString("CPU"));
    case COLUMN_GPU_TIME:
      return QVariant(QString("GPU"));
    case COLUMN_FILE:
      return QVariant(QString("File"));
    case COLUMN_FUNCTION:
      return QVariant(QString("Function"));
    case COLUMN_NAME:
      return QVariant(QString("Name"));
    case COLUMN_LINE:
      return QVariant(QString("Line"));
    default:
      return QVariant();
    }
  }

  void print(const QModelIndex& index=QModelIndex())
  {
    if(index.isValid())
    {
      widget->print(*reinterpret_cast<const DataLine*>(index.internalPointer()));
    }else
    {
      qDebug() << "//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\\\\";
    }

    int nRows = rowCount(index);

    for(int i=0; i<nRows; ++i)
    {
      QModelIndex child = this->index(i, 0, index);
      if(parent(child) != index)
        qCritical() << "!!!!!!!!!!!!!Parent/Child mismatch!!!!!!!!!!!!";
      print(child);
    }

    if(!index.isValid())
      qDebug() << "\\\\++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//\n\n";
  }

  void sendModelStartChangeSignal()
  {
    beginResetModel();
  }

  void sendModelChangedEndSignal()
  {
    endResetModel();
  }
};

QAbstractItemModel* ConnectionWidget::createModel()
{
  return model = new DataModel(this);
}

void ConnectionWidget::sendModelStartChangeSignal()
{
  model->sendModelStartChangeSignal();
}

void ConnectionWidget::sendModelChangedEndSignal()
{
  model->sendModelChangedEndSignal();
  ui->treeView->expandAll();

  //printReceivedData();
  //model->print();
}

void ConnectionWidget::printReceivedData()
{
  qDebug() << "//----------------------------------------------------------------------------\\\\";
  for(const DataLine& data : currentData)
    print(data);
  qDebug() << "\\\\----------------------------------------------------------------------------//\n\n";
}

void ConnectionWidget::print(const DataLine& data)
{
  QString line;

  for(int i=0; i<data.depth; ++i)
    line += "    ";

  std::string str = QString("%0%1  %2  %3 `%4` `%5` %6")
                    .arg(line)
                    .arg(strings.value(data.name, "???"))
                    .arg(data.cpuTime!=MAX_TIME ? QString("%0").arg(data.cpuTime) : QString("--"))
                    .arg(data.gpuTime!=MAX_TIME ? QString("%1").arg(data.gpuTime) : QString("--"))
                    .arg(strings.value(data.function, "???"))
                    .arg(strings.value(data.file, "???"))
                    .arg(data.line).toStdString();
  qDebug() << str.c_str();
}
