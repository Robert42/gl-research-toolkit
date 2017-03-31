#! /usr/bin/env python3

import os
import shutil

script_dir = os.path.dirname(os.path.abspath(__file__))

target_dir = os.path.join(script_dir, "software")
shutil.rmtree(target_dir, ignore_errors=True)
os.mkdir("software")

def copy_tree(name):
  shutil.copytree(os.path.join(script_dir, name), os.path.join(target_dir, name))
def copy_file(name):
  shutil.copy(os.path.join(script_dir, name), os.path.join(target_dir, name))

copy_tree("cmake")
copy_tree("doc")
copy_tree("external")
copy_tree("gl-research-toolkit")
copy_tree("samples")
copy_tree("shader")
copy_tree("tests")
copy_tree("tools")
copy_file("CMakeLists.txt")
#copy_file("LICENSE.md")
copy_file("README.md")
