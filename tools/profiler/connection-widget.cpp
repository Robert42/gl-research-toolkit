#include "connection-widget.h"
#include "ui_connection-widget.h"

#include <glrt/toolkit/network.h>

ConnectionWidget::ConnectionWidget(QTcpSocket* tcpSocket, QWidget *parent) :
  QWidget(parent),
  applicationName("Connected Game"),
  ui(new Ui::ConnectionWidget),
  tcpSocket(tcpSocket)
{
  ui->setupUi(this);

  ui->treeView->setModel(this->createModel());

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

  DataModel(ConnectionWidget* w)
    : QAbstractItemModel(w),
      currentData(w->currentData),
      strings(w->strings)
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
      if(r == row)
        return createIndex(row, column, childRow);

      if(childRow->depth == expectedDepth)
        r++;
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
    DataLine* row = childRow;

    while(row >= firstRow)
    {
      if(row->depth == childRow->depth-1)
        break;

      row--;
    }

    DataLine* parentRow = row;
    int rowIndex = 0;

    while(row >= firstRow)
    {
      if(row->depth == parentRow->depth-1)
        return createIndex(rowIndex, child.column(), parentRow);

      if(row->depth == parentRow->depth)
        rowIndex++;
      row--;
    }

    return QModelIndex();
  }

  int rowCount(const QModelIndex& parent) const override
  {
    DataLine* dataLine = firstChildOf(parent);

    if(dataLine == nullptr)
      return 0;

    DataLine* lastRow = &currentData.last();
    int expectedDepth = dataLine->depth;
    int nRows = 1;

    while(lastRow>=dataLine && dataLine->depth>=expectedDepth)
    {
      lastRow++;
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
        return QVariant(data.cpuTime);
      case COLUMN_GPU_TIME:
        return QVariant(data.gpuTime);
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
}
