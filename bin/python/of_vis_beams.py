# -*- coding: iso-8859-15 -*-

import numpy as np
import calfem.core as cfc
import calfem.utils as cfu
import vedo as vd

class VedoCanvas:
    """
    Class for handling shared state for the vis_vedo functions.
    """
    __instance = None
    @staticmethod
    def instance():
        """Static access method."""
        if VedoCanvas.__instance == None: VedoCanvas()
        
        return VedoCanvas.__instance

    def __init__(self):
        """Virtually private constructor."""
        if VedoCanvas.__instance is None:
            VedoCanvas.__instance = self

            self.__entities = []
            self.__current_color = "yellow"
            self.__current_alpha = 1.0
            self.__current_tube_radius = 0.025

    def add(self, vedo_object):
        self.__entities.append(vedo_object)

    
    def save(self, filename):
        print(self.__entities)
        vd.write(self.__entities, filename)

    @property
    def entities(self):
        return self.__entities
    
    @property
    def current_color(self):
        return self.__current_color
    
    @current_color.setter
    def current_color(self, value):
        self.__current_color = value
    
    @property
    def current_alpha(self):
        return self.__current_alpha
    
    @current_alpha.setter
    def current_alpha(self, value):
        self.__current_alpha = value

    @property
    def current_tube_radius(self):
        return self.__current_tube_radius
    
    @current_tube_radius.setter
    def current_tube_radius(self, value):
        self.__current_tube_radius = value
    
def vis_canvas():
    return VedoCanvas.instance() 

def add_canvas(vedo_object):
    vis_canvas().add(vedo_object)

def save(filename):
    vis_canvas().save(filename)

def set_color(color):
    vis_canvas().current_color = color

def set_alpha(alpha):
    vis_canvas().current_alpha = alpha

def set_color_and_alpha(color, alpha):
    vis_canvas().current_color = color
    vis_canvas().instance().current_alpha = alpha

def set_tube_radius(r):
    vis_canvas().current_tube_radius = r


def current_color():
    return vis_canvas().current_color

def current_alpha():
    return vis_canvas().current_alpha

def current_tube_radius():
    return vis_canvas().current_tube_radius


def draw_beams(edof, ex, ey, ez):
    coords, topo, node_dofs = cfu.convert_to_node_topo(edof, ex, ey, ez, n_dofs_per_node=6, ignore_first=False)

    for el_topo in topo:
        l = vd.Line(coords[el_topo[0]], coords[el_topo[1]])
        l.linewidth(2)
        t = vd.Tube([coords[el_topo[0]], coords[el_topo[1]]], r=current_tube_radius(), c=current_color(), alpha=current_alpha())
        add_canvas(t)

def draw_bars(edof, ex, ey, ez):
    coords, topo, node_dofs = cfu.convert_to_node_topo(edof, ex, ey, ez, n_dofs_per_node=3, ignore_first=False)

    lines = []

    for el_topo in topo:
        l = vd.Line(coords[el_topo[0]], coords[el_topo[1]])
        l.linewidth(2)
        t = vd.Tube([coords[el_topo[0]], coords[el_topo[1]]], r=current_tube_radius(), c=current_color(), alpha=current_alpha())
        add_canvas(t)

def draw_beam_displacements(a, coords, edof, dofs, magnfac=100):
    
    ex, ey, ez = cfc.coord_extract(edof, coords, dofs)
    ed = cfc.extract_eldisp(edof, a)

    coords, topo, node_dofs = cfu.convert_to_node_topo(edof, ex, ey, ez, n_dofs_per_node=6, ignore_first=False)

    for el_topo in topo:
        p0 = coords[el_topo[0]] 
        p1 = coords[el_topo[1]]
        d0 = np.array(a[node_dofs[el_topo[0]][:3]-1]).flatten()
        d1 = np.array(a[node_dofs[el_topo[1]][:3]-1]).flatten()

        t = vd.Tube([p0+d0*magnfac, p1+d1*magnfac], r=current_tube_radius(), c=current_color(), alpha=current_alpha())
        add_canvas(t)
    
def draw_bar_displacements(a, coords, edof, dofs, magnfac=100):
    
    ex, ey, ez = cfc.coord_extract(edof, coords, dofs)
    ed = cfc.extract_eldisp(edof, a)

    coords, topo, node_dofs = cfu.convert_to_node_topo(edof, ex, ey, ez, n_dofs_per_node=3, ignore_first=False)

    for el_topo in topo:
        p0 = coords[el_topo[0]] 
        p1 = coords[el_topo[1]]
        d0 = np.array(a[node_dofs[el_topo[0]][:3]-1]).flatten()
        d1 = np.array(a[node_dofs[el_topo[1]][:3]-1]).flatten()

        t = vd.Tube([p0+d0*magnfac, p1+d1*magnfac], r=current_tube_radius(), c=current_color(), alpha=current_alpha())
        add_canvas(t)

def show_and_wait():
    plt = vd.Plotter(axes=1, bg2='lightblue')
    plt.look_at(plane="xy")
    plt.show(VedoCanvas.instance().entities, __doc__, viewup='z')
    plt.close()

