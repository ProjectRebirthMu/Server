using System;
using System.Collections.Generic;
using System.Text;

using System.IO;
using System.Threading;
using System.Diagnostics;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace ServerStartUp
{
    public class AppMng
    {
        [DllImport("user32.dll", EntryPoint = "ShowWindow", SetLastError = true)]
        static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool IsWindowVisible(IntPtr hWnd);

        public const int MaxApplications = 32;

        public ProcessStartInfo[] ListInfo = new ProcessStartInfo[MaxApplications];
        public Process[] ListProc = new Process[MaxApplications];

        public void Run(int Index, string FilePath, string Args, int Delay,int WindowStyle )
        {
            try
            {
                if (File.Exists(FilePath))
                {
                    if (isRunning(Index) == false)
                    {
                        ListInfo[Index] = new ProcessStartInfo();

                        ListInfo[Index].WorkingDirectory = Path.GetDirectoryName(FilePath);
                        ListInfo[Index].FileName = FilePath;
                        ListInfo[Index].Arguments = Args;
                        ListInfo[Index].WindowStyle = ProcessWindowStyle.Normal;
                        ListInfo[Index].UseShellExecute = false;

                        ListProc[Index] = Process.Start(ListInfo[Index]);
                        ListProc[Index].Refresh();

                        if (Delay > 0)
                        {
                            Thread.Sleep(Delay);
                        }

                        ShowSelectedWindow(Index, WindowStyle);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        public Boolean Kill(int Index)
        {
            try
            {
                if (ListProc[Index] != null)
                {
                    ListProc[Index].Kill();
                    ListProc[Index] = null;
                    return true;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }

            return false;
        }

        public Boolean KillAll()
        {
            Boolean Result = false;
            for (int i = 0; i < MaxApplications; i++)
            {
                if (ListProc[i] != null)
                {
                    if (ListProc[i].HasExited == false)
                    {
                        ListProc[i].Kill();
                        ListProc[i] = null;
                        Result = true;
                    }
                }
            }

            return Result;
        }

        public Boolean isNull(int Index)
        {
            if (ListProc[Index] != null)
            {
                return false;
            }

            return true;
        }

        public Boolean isRunning(int Index)
        {
            if (ListProc[Index] != null)
            {
                if (ListProc[Index].HasExited == false)
                {
                    return true;
                }
            }

            return false;
        }

        public Boolean AnyRunning()
        {
            try
            {
                foreach (Process proc in ListProc)
                {
                    if (proc != null)
                    {
                        if (proc.HasExited == false)
                        {
                            return true;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }

            return false;
        }

        public int GetCount(Boolean HasExited)
        {
            int Count = 0;

            foreach (Process proc in ListProc)
            {
                if (proc != null)
                {
                    if (proc.HasExited == HasExited)
                    {
                        Count++;
                    }
                }
            }

            return Count;
        }

        public Boolean HideAllWindows()
        {
            try
            {
                Boolean Result = false;
                foreach (Process proc in this.ListProc)
                {
                    if (proc != null)
                    {
                        if (proc.HasExited == false)
                        {
                            Result = true;
                            ShowWindow(proc.MainWindowHandle, 0);
                        }
                    }
                }

                return Result;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }

            return false;
        }

        public Boolean ShowAllWindows()
        {
            try
            {
                Boolean Result = false;
                foreach (Process proc in this.ListProc)
                {
                    if (proc != null)
                    {
                        if (proc.HasExited == false)
                        {
                            Result = true;
                            ShowWindow(proc.MainWindowHandle, 1);
                        }
                    }
                }

                return Result;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }

            return false;
        }

        public Boolean IsWindowVisible(int Index)
        {
            try
            {
                if (this.ListProc[Index] != null)
                {
                    if (this.ListProc[Index].HasExited == false)
                    {
                        return IsWindowVisible(ListProc[Index].MainWindowHandle);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }

            return false;
        }

        public Boolean ShowSelectedWindow(int Index, int State)
        {
            try
            {
                if (this.ListProc[Index] != null)
                {
                    if (this.ListProc[Index].HasExited == false)
                    {

                        return ShowWindow(ListProc[Index].MainWindowHandle, State);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }

            return false;
        }
    }
}
