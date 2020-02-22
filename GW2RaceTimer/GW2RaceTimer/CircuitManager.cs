using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;

namespace GW2RaceTimer
{
    class CircuitManager
    {
        public TimeSpan TotalElapsedTime { get; private set; }
        public DateTime StartTime { get; private set; }
        public bool IsRunning { get; private set; }

        public EventHandler<ElapsedEventArgs> TimedEvent;
        // LastCheckpointTime
        // Race
        Timer circuitTimer;

        public CircuitManager()
        {
            circuitTimer = new Timer(10);
            circuitTimer.Elapsed += OnTimedEvent;
            IsRunning = false;
        }

        public void Start()
        {
            TotalElapsedTime = new TimeSpan(0);
            StartTime = DateTime.Now;
            circuitTimer.Start();
            IsRunning = true;
        }

        public void Cancel()
        {
            circuitTimer.Stop();
            TotalElapsedTime = new TimeSpan(0);
            IsRunning = false;
        }

        private void OnTimedEvent(Object source, ElapsedEventArgs e)
        {
            TotalElapsedTime = e.SignalTime.Subtract(StartTime);

            TimedEvent?.Invoke(source, e);
        }

        // Start
        // Cancel
        // CircuitThread
    }
}
