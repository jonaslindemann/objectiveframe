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
                    time.sleep(3)
                    print("Done.")
                else:
                    print(f"Could not find ObjectiveFrame at {self.executable}.")

    def stop(self):
        self.process.terminate()
        self.process = None

    def _post(self, endpoint, data=None):
        """Send a POST request. data may be a JSON-serialisable object or None."""
        body = json.dumps(data) if data is not None else None
        return requests.post(self.url + endpoint, body)

    def _post_json(self, endpoint, data=None):
        """POST and return the parsed JSON response body."""
        return self._post(endpoint, data).json()

    # ── Model lifecycle ───────────────────────────────────────────────────────

    def new_model(self):
        self._post("/cmds/new_model")

    def open_model(self, filename):
        self._post("/cmds/open_model", os.path.abspath(filename))

    def save_model(self, filename):
        self._post("/cmds/save_model", os.path.abspath(filename))

    def export_calfem(self, filename):
        self._post("/cmds/export_model", os.path.abspath(filename))

    def import_calfem(self, filename):
        self._post("/cmds/import_model", os.path.abspath(filename))

    def snap_shot(self):
        self._post("/cmds/snap_shot")

    # ── Node / beam creation ──────────────────────────────────────────────────

    def add_nodes(self, nodes):
        """nodes: list of [x, y, z] triples."""
        self._post("/cmds/add_nodes", nodes)

    def add_node(self, x, y, z):
        self.add_nodes([[x, y, z]])

    def add_beams(self, beams):
        """beams: list of [i0, i1] index pairs."""
        self._post("/cmds/add_beams", beams)

    def add_beam(self, i0, i1):
        self.add_beams([[i0, i1]])

    # ── Selection ─────────────────────────────────────────────────────────────

    def select_all(self):
        self._post("/cmds/select_all")

    def select_all_nodes(self):
        self._post("/cmds/select_all_nodes")

    def clear_selection(self):
        self._post("/cmds/clear_selection")

    def add_last_node_to_selection(self):
        self._post("/cmds/add_last_node_to_selection")

    def select_node_at(self, index):
        self._post("/cmds/select_node_at", {"index": index})

    def select_beam_at(self, index):
        self._post("/cmds/select_beam_at", {"index": index})

    # ── Node / beam mutation ──────────────────────────────────────────────────

    def delete_node_at(self, index):
        self._post("/cmds/delete_node_at", {"index": index})

    def delete_beam_at(self, index):
        self._post("/cmds/delete_beam_at", {"index": index})

    def subdivide_beam_at(self, index):
        self._post("/cmds/subdivide_beam_at", {"index": index})

    def connect_near_nodes(self, tolerance):
        self._post("/cmds/connect_near_nodes", {"tolerance": tolerance})

    def update_node_pos_at(self, index, x, y, z):
        self._post("/cmds/update_node_pos_at", {"index": index, "pos": [x, y, z]})

    def update_beam_at(self, index, i0, i1):
        self._post("/cmds/update_beam_at", {"index": index, "i0": i0, "i1": i1})

    # ── Boundary conditions ───────────────────────────────────────────────────

    def assign_node_fixed_bc_ground(self):
        self._post("/cmds/assign_node_fixed_bc_ground")

    def assign_node_pos_bc_ground(self):
        self._post("/cmds/assign_node_pos_bc_ground")

    def assign_node_fixed_bc_at(self, index):
        self._post("/cmds/assign_node_fixed_bc_at", {"index": index})

    def assign_node_pos_bc_at(self, index):
        self._post("/cmds/assign_node_pos_bc_at", {"index": index})

    def remove_node_bc_at(self, index):
        self._post("/cmds/remove_node_bc_at", {"index": index})

    # ── Loads ─────────────────────────────────────────────────────────────────

    def clear_all_loads(self):
        self._post("/cmds/clear_all_loads")

    def clear_all_bcs(self):
        self._post("/cmds/clear_all_bcs")

    def add_node_load_at(self, index, fx, fy, fz):
        self._post("/cmds/add_node_load_at", {"index": index, "force": [fx, fy, fz]})

    def clear_node_load_at(self, index):
        self._post("/cmds/clear_node_load_at", {"index": index})

    def add_beam_load_at(self, index, fx, fy, fz):
        self._post("/cmds/add_beam_load_at", {"index": index, "force": [fx, fy, fz]})

    def clear_beam_load_at(self, index):
        self._post("/cmds/clear_beam_load_at", {"index": index})

    # ── Geometry modification ─────────────────────────────────────────────────

    def array_selection(self, count, step=(1.0, 0.0, 0.0), span_step=True,
                        copy_loads=True, tolerance=0.0):
        """Repeat the selection along a direction, keeping the original.

        count      instances including the original, so 1 is a no-op
        step       offset per copy, not the total span
        span_step  measure step in bounding box extents of the selection, so
                   (1, 0, 0) puts each copy one selection length further along x
        copy_loads carry loads and boundary conditions onto the copies. Safe
                   here: a linear array is a pure translation
        tolerance  weld coincident nodes afterwards, 0 to leave them apart
        """
        self._post("/cmds/array_selection",
                   {"count": count, "step": list(step), "span_step": span_step,
                    "copy_loads": copy_loads, "tolerance": tolerance})

    def polar_array_selection(self, count, axis=(0.0, 1.0, 0.0), angle=360.0, origin=0,
                              rotate_copies=True, full_circle=True, copy_loads=True,
                              tolerance=0.0):
        """Repeat the selection around an axis, keeping the original.

        origin        point the axis passes through: 0 world, 3 cursor. An axis
                      through the selection's own centroid spins the copies on
                      top of it
        angle         swept by the array as a whole
        full_circle   step is angle / count, so the last copy stops one step
                      short of the original. False divides by count - 1, putting
                      the first and last instance on the ends of the arc
        rotate_copies False slides the copies along the arc unrotated
        copy_loads    beam loads always come along (their direction is local),
                      but a rotating array skips node loads and directional
                      supports and reports how many
        """
        self._post("/cmds/polar_array_selection",
                   {"count": count, "axis": list(axis), "angle": angle, "origin": origin,
                    "rotate_copies": rotate_copies, "full_circle": full_circle,
                    "copy_loads": copy_loads, "tolerance": tolerance})

    def plane_array_selection(self, plane, count1, step1, count2, step2,
                              span_step=False, copy_loads=True, tolerance=0.0):
        """Repeat the selection across a principal plane, keeping the original.

        plane   0 xy, 1 xz, 2 yz. count1/step1 run along the first named axis of
                the plane and count2/step2 along the second, so an xz grid is
                x repeat, x step, z repeat, z step
        count1  repeats along the first axis, including the original, so a
                4 x 3 grid is 12 instances and 11 copies
        """
        self._post("/cmds/plane_array_selection",
                   {"plane": plane, "count1": count1, "step1": step1,
                    "count2": count2, "step2": step2, "span_step": span_step,
                    "copy_loads": copy_loads, "tolerance": tolerance})

    # ── Mesh generation ───────────────────────────────────────────────────────

    def mesh_selected_nodes(self):
        self._post("/cmds/mesh_selected_nodes")

    def surface_selected_nodes(self):
        self._post("/cmds/surface_selected_nodes")

    # ── Queries ───────────────────────────────────────────────────────────────

    def node_count(self):
        return self._post_json("/cmds/node_count")["value"]

    def beam_count(self):
        return self._post_json("/cmds/beam_count")["value"]

    def node_pos_at(self, index):
        """Returns [x, y, z]."""
        return self._post_json("/cmds/node_pos_at", {"index": index})["pos"]

    def beam_at(self, index):
        """Returns (i0, i1)."""
        data = self._post_json("/cmds/beam_at", {"index": index})
        return data["i0"], data["i1"]

    def find_node_near(self, x, y, z, tolerance):
        """Returns node index, or -1 if not found."""
        return self._post_json("/cmds/find_node_near",
                               {"pos": [x, y, z], "tolerance": tolerance})["index"]

    def is_node_fixed_at(self, index):
        return self._post_json("/cmds/is_node_fixed_at", {"index": index})["value"]

    def is_node_pos_bc_at(self, index):
        return self._post_json("/cmds/is_node_pos_bc_at", {"index": index})["value"]

    def is_node_selected_at(self, index):
        return self._post_json("/cmds/is_node_selected_at", {"index": index})["value"]

    def has_node_load_at(self, index):
        return self._post_json("/cmds/has_node_load_at", {"index": index})["value"]

    def has_beam_load_at(self, index):
        return self._post_json("/cmds/has_beam_load_at", {"index": index})["value"]

    def node_load_count(self):
        return self._post_json("/cmds/node_load_count")["value"]

    def beam_load_count(self):
        return self._post_json("/cmds/beam_load_count")["value"]

    def material_count(self):
        return self._post_json("/cmds/material_count")["value"]

    def model_bounds(self):
        """Returns (min, max) where each is [x, y, z]."""
        data = self._post_json("/cmds/model_bounds")
        return data["min"], data["max"]

    # ── Properties ────────────────────────────────────────────────────────────

    @property
    def executable(self):
        return self.__executable

    @executable.setter
    def executable(self, value):
        self.__executable = value
        _, self.executable_name = os.path.split(self.executable)


# ── Tests ─────────────────────────────────────────────────────────────────────

def test1(of):
    of.new_model()
    nodes = (10.0 - np.random.random(3*200)*20.0).reshape((200,3)).tolist()
    of.add_nodes(nodes)
    of.add_beams([[0,1],[1,2]])
    of.select_all()

def test2(of):
    of.new_model()
    of.open_model("bar_frame5_with_load.df3")

def test3(of):
    of.new_model()
    nodes = (10.0 - np.random.random(3*200)*20.0).reshape((200,3)).tolist()
    of.add_nodes(nodes)
    of.add_beams([[0,1],[1,2]])
    of.save_model("test4.df3")

def test4(of):
    test3(of)
    of.export_calfem("test4.py")

def test5(of):
    of.new_model()
    of.import_calfem("test4.py")

def test_queries(of):
    of.new_model()
    of.add_node(0, 0, 0)
    of.add_node(1, 0, 0)
    of.add_node(0, 1, 0)
    of.add_beam(0, 1)
    of.add_beam(1, 2)

    print("node_count:", of.node_count())
    print("beam_count:", of.beam_count())
    print("node_pos_at(0):", of.node_pos_at(0))
    print("beam_at(0):", of.beam_at(0))
    print("find_node_near(0,0,0, 0.1):", of.find_node_near(0, 0, 0, 0.1))
    bmin, bmax = of.model_bounds()
    print("model_bounds:", bmin, bmax)


if __name__ == "__main__":
    try:
        of = ObjectiveFrame()
        of.executable = "D:\\Users\\Jonas\\Development\\objectiveframe\\bin\\Debug\\objframe_glfwd.exe"
        of.start()

        test1(of)
        test2(of)
        test3(of)
        test4(of)
        test5(of)
        test_queries(of)

    except requests.exceptions.ConnectionError:
        print("Couldn't connect to ObjectiveFrame. Is it started?")
