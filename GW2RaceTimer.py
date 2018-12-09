import json
import argparse
import struct
import time
import mmap
from scipy.spatial import distance


with open("race_checkpoints.json", "r") as f:
    racedata = json.loads(f.read())

parser = argparse.ArgumentParser(description="")
parser.add_argument("-t", "--tickrate", type=float, default=60, required=False, help="How often to check position updates (higher is more often)")
parser.add_argument("-r", "--radius", type=float, default=7.5, required=False, help="Radius threshold for checkpoints (game uses ~7.5, increasing this may mess with splits)")

args = parser.parse_args()

tickrate = 1 / args.tickrate
radius = args.radius

# Main loop
# TODO: close when GW2 closes?
while True:
    points = racedata["lakeside"] # TODO: load based on mapId here

    i = 0
    last = None
    starttime = 0
    first = True
    # Race loop
    while i < len(points):
        time.sleep(tickrate)

        shmem = mmap.mmap(0, 20, "MumbleLink", mmap.ACCESS_READ)
        coord = struct.unpack("IL3f", shmem)[2:5]
        shmem.close()

        # Drop this if we haven't moved
        if last == coord:
            continue
        last = coord

        if distance.euclidean(coord, points[i]) <= radius:
            if first:
                starttime = time.time()
                first = False
            i += 1
            print("Gate {}: {}".format(i, time.time() - starttime))
