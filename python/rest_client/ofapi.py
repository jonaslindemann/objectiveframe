# -*- coding: utf-8 -*-
"""
ObjectiveFrame REST API
"""

import requests
import json
import numpy as np

    # m_webServer->addHandler("/cmds/add_nodes", m_addNodesHandler);
    # m_webServer->addHandler("/cmds/add_beams", m_addBeamsHandler);
    # m_webServer->addHandler("/cmds/new_model", m_newModelHandler);
    # m_webServer->addHandler("/cmds/mesh_selected_nodes", m_meshSelectedNodesHandler);
    # m_webServer->addHandler("/cmds/surface_selected_nodes", m_surfaceSelectedNodesHandler);
    # m_webServer->addHandler("/cmds/select_all", m_selectAllHandler);
    # m_webServer->addHandler("/cmds/clear_selection", m_clearSelectionHandler);
    # m_webServer->addHandler("/cmds/assign_node_fixed_bc_ground", m_assignNodeFixedBCGroundHandler);
    # m_webServer->addHandler("/cmds/assign_node_pos_bc_ground", m_assignNodePosBCGroundHandler);
    # m_webServer->addHandler("/cmds/add_last_node_to_selection", m_addLastNodeToSelectionHandler);


class ObjectiveFrame:
    def __init__(self, url="http://localhost:8081"):
        self.session = requests.Session()
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

    of = ObjectiveFrame()

    of.new_model()

    nodes = (10.0 - np.random.random(3*200)*20.0).reshape((200,3)).tolist()

    of.add_nodes(nodes)

    beams = [
        [0,1],
        [1,2]
    ]

    of.add_beams(beams)