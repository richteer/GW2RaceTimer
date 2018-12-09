import sys
import json
from scipy.spatial import distance
import mmap
import struct
import time


with open("race_fixme.json", "r") as f:
    racedata = json.loads(f.read())

if len(sys.argv) != 2:
    print("Available races: " + ", ".join(racedata.keys()))
    quit()

points = racedata.get(sys.argv[1])

if not points:
    print("Available races: " + ", ".join(racedata.keys()))
    quit()

radius = 10
tickrate = 1/120
i = 0
hit = [-1 for _ in range(len(points))] # array of indicies to sort by
last = None
first = True

while i < len(points):
    time.sleep(tickrate)
    shmem = mmap.mmap(0, 20, "MumbleLink", mmap.ACCESS_READ)
    coord = struct.unpack("IL3f", shmem)[2:5]

    shmem.close()
    if last == coord:
        continue # haven't moved
    last = coord # but now we did

    for p in range(len(points)):
        if distance.euclidean(coord, points[p]) <= radius and hit[p] == -1:
            if first:
                first = False
                starttime = time.time()
            #print("hit point p {} at index {}".format(str(points[p]), p))
            #hit.add(points[p])
            hit[p] = i
            i += 1
            print("Gate {}: {}".format(i, time.time() - starttime))
            break

points = racedata.get(sys.argv[1])

sorteddata = [i for _, i in sorted(zip(hit,points), key=lambda x: x[0])]

if sorteddata == points:
    print("Race checkpoints are confirmed sorted!")
else:
    print("Writing race data to file...")
    for i in range(len(hit)):
        if i != hit[i]:
            print("{} -> {}".format(i, hit[i]))

racedata[sys.argv[1]] = sorteddata

with open("race_fixme.json", "w") as f:
    f.write(json.dumps(racedata))

