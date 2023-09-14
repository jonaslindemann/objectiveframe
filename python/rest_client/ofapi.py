# -*- coding: utf-8 -*-
"""
ObjectiveFrame REST API
"""

import requests
import json
import numpy as np

class ObjectiveFrame:
    def __init__(self, url="http://localhost:8081"):
        self.session = requests.Session()
        self.url = url

    def addNodes(self, nodes):
        response = self.session.post(self.url+"/cmds/addnodes", json.dumps(nodes))

    def addNode(self, x, y, z):
        self.addNodes([[x, y, z]])

    def addBeams(self, beams):
        response = self.session.post(self.url+"/cmds/addbeams", json.dumps(beams))

if __name__ == "__main__":

    of = ObjectiveFrame()

    nodes = (10.0 - np.random.random(3*200)*20.0).reshape((200,3)).tolist()

    of.addNodes(nodes)

    beams = [
        [0,1],
        [1,2]
    ]

    of.addBeams(beams)