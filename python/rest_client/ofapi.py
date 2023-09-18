# -*- coding: utf-8 -*-
"""
ObjectiveFrame REST API
"""

import requests
import json
import logging
import numpy as np

import http.client



class ObjectiveFrame:
    def __init__(self, url="http://localhost:8081"):
        self.session = requests.Session()
        self.session.verify = True
        self.url = url

    def add_nodes(self, nodes):
        response = self.session.post(self.url+"/cmds/add_nodes", json.dumps(nodes))

    def add_node(self, x, y, z):
        self.addNodes([[x, y, z]])

    def add_beams(self, beams):
        response = self.session.post(self.url+"/cmds/add_beams", json.dumps(beams))

    def new_model(self):
        response = self.session.post(self.url+"/cmds/new_model")

    def mesh_selected_nodes(self):
        response = self.session.post(self.url+"/cmds/mesh_selected_nodes")

    def surface_selected_nodes(self):
        response = self.session.post(self.url+"/cmds/surface_selected_nodes")

    def select_all(self):
        response = self.session.post(self.url+"/cmds/select_all")

    def clear_selection(self):
        response = self.session.post(self.url+"/cmds/clear_selection")

    def assign_node_fixed_bc_ground(self):
        response = self.session.post(self.url+"/cmds/assign_node_fixed_bc_ground")

    def assign_node_pos_bc_ground(self):
        response = self.session.post(self.url+"/cmds/assign_node_pos_bc_ground")
    
    def add_last_node_to_selection(self):
        response = self.session.post(self.url+"/cmds/add_last_node_to_selection")


if __name__ == "__main__":

    http.client.HTTPConnection.debuglevel = 1

    logging.basicConfig()
    logging.getLogger().setLevel(logging.DEBUG)
    requests_log = logging.getLogger("requests.packages.urllib3")
    requests_log.setLevel(logging.DEBUG)
    requests_log.propagate = True    

    of = ObjectiveFrame()

    of.new_model()

    nodes = (10.0 - np.random.random(3*200)*20.0).reshape((200,3)).tolist()

    of.add_nodes(nodes)

    beams = [
        [0,1],
        [1,2]
    ]

    of.add_beams(beams)
    of.select_all()