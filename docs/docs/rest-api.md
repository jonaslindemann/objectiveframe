# Automation API

ObjectiveFrame can be driven from another program over a small HTTP service. It is the same set of commands the user interface and the scripts use, so a Python program can build a model, solve it, and read results back while the application stays on screen and keeps drawing.

This is what the Python client in `python/rest_client/ofapi.py` talks to.

## Starting the service

The service is off by default. Start it from either:

- the start page, with **Start automation API**, or
- **File / Preferences...**, with the **Start service** button.

It then listens on `http://localhost:8081`.

!!! warning

    The service has no authentication and is intended for a local, trusted session. It accepts any request that reaches it. Do not expose port 8081 beyond the machine ObjectiveFrame runs on.

## Calling it

Every command is a `POST` to `/cmds/<name>`. Arguments go in the request body, as JSON for most commands and as a plain filename string for the file commands.

Commands that only act return `200 OK` with an empty `text/html` body. Queries return `application/json`:

| Shape | Returned by |
| --- | --- |
| `{"value": n}` | Counts, indices, and boolean queries |
| `{"pos": [x, y, z]}` | `node_pos_at` |
| `{"i0": i0, "i1": i1}` | `beam_at` |
| `{"min": [...], "max": [...]}` | `model_bounds` |

Most handlers read their arguments strictly, so a missing field is an error. The three array commands are the exception: only the count is required, and everything else falls back to the same defaults the command itself uses — `{"count": 4}` is a complete request.

```python
import json, requests

url = "http://localhost:8081"

requests.post(url + "/cmds/new_model")
requests.post(url + "/cmds/add_nodes", json.dumps([[0, 0, 0], [4, 0, 0], [2, 3, 0]]))
requests.post(url + "/cmds/add_beams", json.dumps([[0, 1], [0, 2], [1, 2]]))
requests.post(url + "/cmds/assign_node_fixed_bc_ground")
requests.post(url + "/cmds/add_node_load_at",
              json.dumps({"index": 2, "force": [0.0, -1000.0, 0.0]}))

n = requests.post(url + "/cmds/node_count").json()["value"]
print(n, "nodes")
```

## Commands

### Model lifecycle

| Endpoint | Body |
| --- | --- |
| `new_model` | — |
| `open_model` | filename |
| `save_model` | filename |
| `export_model` | filename, as CALFEM for Python |
| `import_model` | filename, from CALFEM for Python |
| `snap_shot` | — (places an undo point) |

### Creating nodes and elements

| Endpoint | Body |
| --- | --- |
| `add_nodes` | `[[x, y, z], ...]` |
| `add_beams` | `[[i0, i1], ...]` |
| `mesh_selected_nodes` | — |
| `surface_selected_nodes` | — |

### Selection

| Endpoint | Body |
| --- | --- |
| `select_all` | — |
| `select_all_nodes` | — |
| `clear_selection` | — |
| `add_last_node_to_selection` | — |
| `select_node_at` | `{"index": i}` |
| `select_beam_at` | `{"index": i}` |

### Editing nodes and elements

| Endpoint | Body |
| --- | --- |
| `delete_node_at` | `{"index": i}` |
| `delete_beam_at` | `{"index": i}` |
| `subdivide_beam_at` | `{"index": i}` |
| `connect_near_nodes` | `{"tolerance": t}` |
| `update_node_pos_at` | `{"index": i, "pos": [x, y, z]}` |
| `update_beam_at` | `{"index": i, "i0": i0, "i1": i1}` |

### Boundary conditions

| Endpoint | Body |
| --- | --- |
| `assign_node_fixed_bc_ground` | — |
| `assign_node_pos_bc_ground` | — |
| `assign_node_fixed_bc_at` | `{"index": i}` |
| `assign_node_pos_bc_at` | `{"index": i}` |
| `remove_node_bc_at` | `{"index": i}` |
| `clear_all_bcs` | — |

### Loads

| Endpoint | Body |
| --- | --- |
| `add_node_load_at` | `{"index": i, "force": [fx, fy, fz]}` |
| `clear_node_load_at` | `{"index": i}` |
| `add_beam_load_at` | `{"index": i, "force": [fx, fy, fz]}` |
| `clear_beam_load_at` | `{"index": i}` |
| `clear_all_loads` | — |

### Self-weight

| Endpoint | Body |
| --- | --- |
| `set_self_weight_enabled` | `{"enabled": true}` |
| `set_self_weight_mode` | `{"mode": m}` — 0 material density, 1 total load, 2 mass per length |
| `set_gravity` | `{"value": g}`, with an optional `"scale"` for the load factor |
| `set_total_weight` | `{"value": w}` |
| `set_mass_per_length` | `{"value": m}` |

### Geometry modification

These act on the current selection, so select something first.

| Endpoint | Body |
| --- | --- |
| `array_selection` | `{"count": n}`, optionally `step`, `span_step`, `copy_loads`, `tolerance` |
| `polar_array_selection` | `{"count": n}`, optionally `axis`, `angle`, `origin`, `rotate_copies`, `full_circle`, `copy_loads`, `tolerance` |
| `plane_array_selection` | `{"count1": n1, "count2": n2}`, optionally `plane`, `step1`, `step2`, `span_step`, `copy_loads`, `tolerance` |

Counts include the original, and steps are per copy. See [Using ObjectiveFrame](use.md#repeating-a-selection-with-arrays) for what the options mean, and [ChaiScript scripting](chaiscript.md#geometry-commands) for the same commands from a script.

### Queries

| Endpoint | Body | Returns |
| --- | --- | --- |
| `node_count` | — | `{"value": n}` |
| `beam_count` | — | `{"value": n}` |
| `node_pos_at` | `{"index": i}` | `{"pos": [x, y, z]}` |
| `beam_at` | `{"index": i}` | `{"i0": i0, "i1": i1}` |
| `find_node_near` | `{"pos": [x, y, z], "tolerance": t}` | `{"value": i}`, or -1 |
| `is_node_fixed_at` | `{"index": i}` | `{"value": bool}` |
| `is_node_pos_bc_at` | `{"index": i}` | `{"value": bool}` |
| `is_node_selected_at` | `{"index": i}` | `{"value": bool}` |
| `has_node_load_at` | `{"index": i}` | `{"value": bool}` |
| `has_beam_load_at` | `{"index": i}` | `{"value": bool}` |
| `node_load_count` | — | `{"value": n}` |
| `beam_load_count` | — | `{"value": n}` |
| `material_count` | — | `{"value": n}` |
| `model_bounds` | — | `{"min": [...], "max": [...]}` |

## The Python client

`python/rest_client/ofapi.py` wraps these endpoints in an `ObjectiveFrame` class, and can also start the application if it is not already running:

```python
from ofapi import ObjectiveFrame

of = ObjectiveFrame()
of.start()

of.new_model()
of.add_nodes([[0, 0, 0], [4, 0, 0], [2, 3, 0]])
of.add_beams([[0, 1], [0, 2], [1, 2]])
of.assign_node_fixed_bc_ground()
of.add_node_load_at(2, 0.0, -1000.0, 0.0)

print(of.node_count(), "nodes")
print(of.model_bounds())
```

The client wraps the model, selection, boundary condition, load, array and query commands. The self-weight endpoints are currently only reachable by posting to them directly.
