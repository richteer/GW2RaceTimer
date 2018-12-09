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
parser.add_argument("-r", "--radius", type=float, default=10, required=False, help="Radius threshold for checkpoints (game uses ~7.5, increasing this may mess with splits)")

args = parser.parse_args()

tickrate = 1 / args.tickrate
radius = args.radius

shmem = mmap.mmap(0, 20, "MumbleLink", mmap.ACCESS_READ)
print("Connected!")

def formattime(x):
    return "{0: 2d}:{1:06.3f}".format(int(x // 60), x % 60)

try:
    with open("racehistory.json", "r") as f:
        racehistory = json.loads(f.read())
except FileNotFoundError:
    racehistory = {}

# Main loop
# TODO: close when GW2 closes?
while True:
    racemap = "lakeside"  # TODO: load based on mapId here
    points = racedata[racemap]
    previous = racehistory.get(racemap, [None])[-1]

    i = 0
    last = None
    starttime = 0
    first = True
    print("Waiting for first checkpoint...")
    # Race loop
    while i < len(points):
        time.sleep(tickrate)
        if starttime:
            print("\r{0}".format(formattime(time.time() - starttime)), end="")

        coord = struct.unpack("IL3f", shmem)[2:5]

        # Drop this if we haven't moved
        if last == coord:
            continue
        last = coord

        if distance.euclidean(coord, points[i]) <= radius:
            # Handle the first gate a little differently - start the timer because that's the best we can do for now
            if first:
                starttime = time.time()
                times = [0.0] # TODO: this is here so the array is balanced, but we shouldn't need this
                first = False
                print("Timer start!")
                i += 1
                continue

            times.append(time.time() - starttime)
            if previous:
                splitchange = "[{0:+.3f}]".format(times[i] - previous[i])
            else:
                splitchange = ""
            print("\rGate {0:2d}: {1}  {2}".format(i, formattime(times[i]), splitchange))
            i += 1

    print("Race complete!")
    # TODO: clean this mess up. probably better ways to manage the history
    if not previous:
        previous = times
        racehistory[racemap] = [times]
    elif times[-1] < previous[-1]:
        print("New record!")
        previous = times
        racehistory[racemap].append(times)
    with open("racehistory.json", "w") as f:
        f.write(json.dumps(racehistory))

    time.sleep(3) # Cooldown so we don't accidentally start over

    print("")
shmem.close()
