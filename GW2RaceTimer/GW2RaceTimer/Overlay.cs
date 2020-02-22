using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Timers;
using GW2RaceTimer.Structs;

namespace GW2RaceTimer
{
    using HWND = IntPtr;
    public partial class Overlay : Form, INotifyPropertyChanged
    {

        private WindowManager guildWarsWindow;
        private WindowManager currentWindow;
        private CircuitManager circuitManager;
        private Thread positioningThread;
        private string displayTime;

        public string DisplayTime
        {
            get { return displayTime; }
            set
            {
                displayTime = value;
                OnPropertyChanged("DisplayTime");
            }
        }

        public Overlay()
        {
            InitializeComponent();
            displayTime = "00:00.00";

            lblTimer.DataBindings.Add(new Binding("Text", this, "DisplayTime"));

            currentWindow = new WindowManager(this.Handle);
            guildWarsWindow = new WindowManager("ArenaNet_Dx_Window_Class", "Guild Wars 2");

            // Make window's borders transparent
            //currentWindow.SetWindowLong(GWL_EXSTYLE, (IntPtr)(currentWindow.GetWindowLong(GWL_EXSTYLE) ^ WS_EX_LAYERED ^ WS_EX_TRANSPARENT));
            currentWindow.SetWindowLong(GWL_EXSTYLE, (IntPtr)(currentWindow.GetWindowLong(GWL_EXSTYLE) ^ WS_EX_LAYERED));

            // Set the alpha for the whole window to 255
            //SetLayeredWindowAttributes(this.Handle, 0, 255, LWA_ALPHA);
            currentWindow.SetLayeredWindowAttributes(0, 0, LWA_COLORKEY);

            positioningThread = new Thread(new ThreadStart(PositioningThread));
            positioningThread.Start();

            circuitManager = new CircuitManager();
            circuitManager.TimedEvent += OnTimedEvent; // TODO: Remove this somewhere
        }

        private void PositioningThread()
        {
            bool bTop = false;
            bool bMin = false;

            WINDOWINFO GWWndInfo;
            WINDOWINFO CurrentWndInfo = currentWindow.WindowInfo;
            WINDOWPLACEMENT CurrentWndPlacement;
            WINDOWPLACEMENT GWWndPlacement;
            RECT GWRect;

            if (guildWarsWindow.Handle != HWND.Zero)
            {
                guildWarsWindow.SetForegroundWindow();
            }

            while (guildWarsWindow.Handle != HWND.Zero)
            {
                GWWndInfo = guildWarsWindow.WindowInfo;
                CurrentWndInfo = currentWindow.WindowInfo;
                CurrentWndPlacement = currentWindow.WindowPlacement;
                GWWndPlacement = guildWarsWindow.WindowPlacement;
                GWRect = guildWarsWindow.WindowRect;

                // If GW2 is active, put overlay on top
                // TODO: readjust to be exactly on top of GW2's window rect
                if (GWWndInfo.dwWindowStatus == WINDOWINFO.WS_ACTIVECAPTION)
                {
                    currentWindow.ShowWindow((int)guildWarsWindow.WindowPlacement.ShowCmd);

                    if (GWWndPlacement.ShowCmd == ShowWindowCommands.Normal || GWWndPlacement.ShowCmd == ShowWindowCommands.Maximize)
                    {
                        POINT gwTopLeft = new POINT(GWRect.Left, GWRect.Top);
                        POINT gwBottomRight = new POINT(GWRect.Right, GWRect.Bottom);
                        // Get screen coordinates
                        guildWarsWindow.ClientToScreen(ref gwTopLeft);
                        guildWarsWindow.ClientToScreen(ref gwBottomRight);

                        currentWindow.SetWindowPos(
                            WindowManager.HWND_TOPMOST,
                            gwTopLeft.X,
                            gwTopLeft.Y,
                            gwBottomRight.X - gwTopLeft.X,
                            gwBottomRight.Y - gwTopLeft.Y,
                            SetWindowPosFlags.DoNotActivate);
                    }

                    bTop = true;
                }
                else if (bTop != false)
                {
                    // Remove TOPMOST attribute but keep overlay on top of GW2
                    currentWindow.SetWindowPos(guildWarsWindow.Handle, 0, 0, 0, 0, (SetWindowPosFlags)(SWP.NOMOVE | SWP.NOSIZE | SWP.NOACTIVATE));
                    guildWarsWindow.SetWindowPos(currentWindow.Handle, 0, 0, 0, 0, (SetWindowPosFlags)(SWP.NOMOVE | SWP.NOSIZE | SWP.NOACTIVATE));
                    bTop = false;
                }
                // Minimize overlay if GW2 is minimized
                if (GWWndPlacement.ShowCmd == ShowWindowCommands.Minimize && bMin != false)
                {
                    currentWindow.ShowWindow((int)ShowWindowCommands.Minimize);
                    bMin = false;
                }
            }
        }

        private void Overlay_Paint(object sender, PaintEventArgs e)
        {
            //Create a margin (the whole form)
            //marg.Left = 0;
            //marg.Top = 0;
            //marg.Right = this.Width;
            //marg.Bottom = this.Height;

            //Expand the Aero Glass Effect Border to the WHOLE form.
            // since we have already had the border invisible we now
            // have a completely invisible window - apart from the DirectX
            // renders NOT in black.
            //DwmExtendFrameIntoClientArea(this.Handle, ref marg);
        }

        public const int GWL_EXSTYLE = -20;

        public const int WS_EX_LAYERED = 0x80000;

        public const int WS_EX_TRANSPARENT = 0x20;

        public const int LWA_ALPHA = 0x2;

        public const int LWA_COLORKEY = 0x1;

        private void Start_Click(object sender, EventArgs e)
        {
            Button startButtom = (Button)sender;

            if (!circuitManager.IsRunning) {
                circuitManager.Start();
                startButtom.Text = "Stop";
            }
            else
            {
                circuitManager.Cancel();
                startButtom.Text = "Start";
            }
        }

        protected virtual void OnPropertyChanged(string property)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(property));
            }
        }

        private void OnTimedEvent(Object source, ElapsedEventArgs e)
        {
            this.Invoke(new Action(() => 
            { 
                DisplayTime = circuitManager.TotalElapsedTime.ToString(@"mm\:ss\.fff"); 
            }
            ));
            
        }

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        #endregion
    }
}
