import json
import argparse
import struct
import time
import mmap
from scipy.spatial import distance


with open("race_checkpoints.json", "r") as f:
    racedata = json.loads(f.read())

parser = argparse.ArgumentParser(description="", epilog="Available maps: "+", ".join(racedata.keys()))
parser.add_argument("-t", "--tickrate", type=float, default=60, required=False, help="How often to check position updates (higher is more often)")
parser.add_argument("-r", "--radius", type=float, default=10, required=False, help="Radius threshold for checkpoints (game uses ~7.5, increasing this may mess with splits)")
parser.add_argument("-s", "--hide-stopwatch", action="store_false", help="Don't print the stopwatch timer to stdout")
parser.add_argument("-S", "--hide-speed", action="store_false", help="Don't print the current velocity estimate (this also disables speed)")
parser.add_argument("map", choices=racedata.keys(), help="Map to load the checkpoint and timer information for")

args = parser.parse_args()

tickrate = 1 / args.tickrate
radius = args.radius
show_stopwatch = args.hide_stopwatch
show_speed = args.hide_speed
racemap = args.map

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
    points = racedata[racemap]
    previous = racehistory.get(racemap)
    if previous:
        # Get your fastest record time
        previous = sorted(previous, key=lambda x: x[-1])[0]

    i = 0
    last = None
    starttime = 0
    first = True
    print("Waiting for first checkpoint...")
    # Race loop
    while i < len(points):
        time.sleep(tickrate)

        coord = struct.unpack("IL3f", shmem)[2:5]

        if starttime and show_stopwatch:
            print("\r{0}".format(formattime(time.time() - starttime)), end="")

        # Drop this if we haven't moved
        if last == coord:
            continue

        if starttime and show_stopwatch and show_speed:
            velocity = distance.euclidean(coord, last) / tickrate
            # Cutoff speed below threshold due to potential floating point inaccuracies, etc
            if velocity < 0.5:
                velocity = 0.0
            print("  {0:6.2f} u/s".format(velocity), end="")
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
        racehistory[racemap] = []
    elif times[-1] < previous[-1]:
        print("New record!")

    racehistory[racemap].append(times)
    with open("racehistory.json", "w") as f:
        f.write(json.dumps(racehistory))

    time.sleep(3) # Cooldown so we don't accidentally start over

    print("")
shmem.close()
