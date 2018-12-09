import xml.etree.ElementTree as ET
import numpy as np
import json

tree = ET.parse('../tw_br_official.xml')
root = tree.getroot()

races = ["ghost", "timetrial", "infernal", "jormags", "lakeside", "valley"]
race_chkpts = {}

for r in races:
    ls = [i.attrib for i in list(list(root.iter("POIs"))[0].iter("POI")) if r in i.attrib["type"] and ("checkpoint" in i.attrib["type"] or "finish" in i.attrib["type"]) ]

    poi_pos = np.array([[ls[i]["xpos"],ls[i]["ypos"],ls[i]["zpos"]] for i in range(len(ls))]).astype(np.float64)

    distance = [(i, j, np.linalg.norm(poi_pos[i]-poi_pos[j])) for i in range(len(poi_pos)) for j in range(len(poi_pos)) if i < j]
    
    if(r == "ghost"):
        race_chkpts[r] = [(poi_pos[i[0]]+poi_pos[i[1]])/2 for i in distance if i[2] < 18]
    elif(r == "timetrial"):
        race_chkpts[r] = [(poi_pos[i[0]]+poi_pos[i[1]])/2 for i in distance if i[2] < 14]
    else:
        race_chkpts[r] = [(poi_pos[i[0]]+poi_pos[i[1]])/2 for i in distance if i[2] < 20]

    race_chkpts[r] = [(float(i), float(j), float(k)) for i,j,k in race_chkpts[r]]

with open("race_fixme.json", "w") as f:
    f.write(json.dumps(race_chkpts))