# -*- coding: utf-8 -*-
"""
ObjectiveFrame REST API
"""

import pycurl_requests as requests

import json, subprocess, os, time, psutil, sys, win32ui
import numpy as np

class ObjectiveFrame:
    def __init__(self, url="http://localhost:8081"):
        self.url = url
        self.__executable = ""

        app_data_dir = os.getenv("LOCALAPPDATA")
        self.executable = os.path.join(app_data_dir, 'Programs\\Structural Mechanics\\ObjectiveFrame\\objframe_glfw.exe')
        self.process = None

    def is_running(self):

        try:
            win32ui.FindWindow("GLFW30", "ObjectiveFrame")
        except win32ui.error:
            return False
        else:
            return True

    def start(self):

        if self.is_running():
            print("ObjectiveFrame already running...")
        else:
            print("ObjectiveFrame not running, starting...")
            if self.process is None:
                if os.path.exists(self.executable):
                    self.process = subprocess.Popen(self.executable)
                    print("Waiting for ObjectiveFrame to start...")
                    time.sleep(5)
                    print("Done.")
                else:
                    print(f"Could not find ObjectiveFrame at {self.executable}.")

    def stop(self):
        self.process.terminate()
        self.process = None

    def add_nodes(self, nodes):
        response = requests.post(self.url+"/cmds/add_nodes", json.dumps(nodes))

    def add_node(self, x, y, z):
        self.addNodes([[x, y, z]])

    def add_beams(self, beams):
        response = requests.post(self.url+"/cmds/add_beams", json.dumps(beams))

    def new_model(self):
        response = requests.post(self.url+"/cmds/new_model")

    def mesh_selected_nodes(self):
        response = requests.post(self.url+"/cmds/mesh_selected_nodes")

    def surface_selected_nodes(self):
        response = requests.post(self.url+"/cmds/surface_selected_nodes")

    def select_all(self):
        response = requests.post(self.url+"/cmds/select_all")

    def clear_selection(self):
        response = requests.post(self.url+"/cmds/clear_selection")

    def assign_node_fixed_bc_ground(self):
        response = requests.post(self.url+"/cmds/assign_node_fixed_bc_ground")

    def assign_node_pos_bc_ground(self):
        response = requests.post(self.url+"/cmds/assign_node_pos_bc_ground")
    
    def add_last_node_to_selection(self):
        response = requests.post(self.url+"/cmds/add_last_node_to_selection")

    def open_model(self, filename):
        response = requests.post(self.url+"/cmds/open_model", os.path.abspath(filename))

    @property
    def executable(self):
        return self.__executable
    
    @executable.setter
    def executable(self, value):
        self.__executable = value
        _, self.executable_name = os.path.split(self.executable)

def test1(of):
    of.new_model()

    nodes = (10.0 - np.random.random(3*200)*20.0).reshape((200,3)).tolist()

    of.add_nodes(nodes)

    beams = [
        [0,1],
        [1,2]
    ]

    of.add_beams(beams)
    of.select_all()

def test2(of):
    of.new_model()
    of.open_model("bar_frame5_with_load.df3")


if __name__ == "__main__":

    try:
        of = ObjectiveFrame()
        of.executable =  "D:\\Users\\Jonas\\Development\\objectiveframe\\bin\\Debug\\objframe_glfwd.exe"
        of.start()

        #test1(of)
        test2(of)

    except requests.exceptions.ConnectionError as err:
        print("Coudn't connect to ObjectiveFrame. Is it started?")
              