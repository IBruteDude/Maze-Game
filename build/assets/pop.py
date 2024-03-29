import json
import math

json.dump({
	'map': 'assets/default.map',
	'player': { 'x': 22, 'y': 12, 'view': 180 * math.pi / 180 },
},
open("save.json", 'w'))
