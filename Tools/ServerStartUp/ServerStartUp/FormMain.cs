using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ServerStartUp
{
    public partial class FormMain : Form
    {
        private AppMng AppManager = new AppMng();
        private DataMng DataManager = new DataMng();

        public int windowStyleOnRun = 0;
        public int processCheckTime = 1;

        public FormMain()
        {
            InitializeComponent();
            
            timerMain.Interval = processCheckTime;

            windowStyleOnRun = (int)DataManager.LoadRegSetting("windowStyleOnRun", windowStyleOnRun);
            processCheckTime = (int)DataManager.LoadRegSetting("processCheckTime", processCheckTime);
        }

        private void timerMain_Tick(object sender, EventArgs e)
        {
            timerMain.Interval = processCheckTime * 1000;

            try
            {
                foreach (DataGridViewRow row in dataGridViewMain.Rows)
                {
                    if (AppManager.isNull(row.Index) == false)
                    {
                        if (Convert.ToBoolean(row.Cells[0].Value) == true)
                        {
                            if (AppManager.isRunning(row.Index) == false)
                            {
                                row.ReadOnly = true;
                                AppManager.Run(row.Index, row.Cells[3].Value.ToString(), row.Cells[4].Value.ToString(), Convert.ToInt32(row.Cells[2].Value), windowStyleOnRun);
                            }
                        }
                    }
                }
            }
            catch (Exception)
            {
                /**/
            }
        }

        private void timerMainGrid_Tick(object sender, EventArgs e)
        {
            try
            {
                foreach (DataGridViewRow row in dataGridViewMain.Rows)
                {
                    bool isRunning = AppManager.isRunning(row.Index);
                    row.ReadOnly = isRunning;
                    row.Cells[1].Value = isRunning ? Properties.Resources.on : Properties.Resources.off;
                }

                statusStripMain.Items[0].Text = AppManager.AnyRunning()
                    ? $" {AppManager.GetCount(false)} app(s) running.."
                    : " not running..";
            }
            catch
            {
                /**/
            }
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!AppManager.AnyRunning())
            {
                try
                {
                    DataManager.SaveDataToXml(dataGridViewMain);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString());
                }

                if (MessageBox.Show(Properties.Resources.Msg_Close, this.Text, MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.No)
                {
                    e.Cancel = true;
                }
            }
            else
            {
                MessageBox.Show(Properties.Resources.Msg_CloseError, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                e.Cancel = true;
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void FormMain_Shown(object sender, EventArgs e)
        {
            try
            {
                DataManager.LoadDataFromXml();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }

            dataGridViewMain.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;

            toolStripButtonDel.Enabled = dataGridViewMain.Rows.Count > 0;
            toolStripButtonDelAll.Enabled = dataGridViewMain.Rows.Count > 0;

            statusStripMain.Items[0].Text = " not running..";
        }

        private void toolStripButtonAdd_Click(object sender, EventArgs e)
        {
            try
            {
                if (openFileDialogMain.ShowDialog() == DialogResult.OK)
                {
                    string fileName = openFileDialogMain.FileName;

                    if (File.Exists(fileName))
                    {
                        dataGridViewMain.Rows.Add(true, Properties.Resources.off, "2000", fileName, "");

                        if (dataGridViewMain.IsCurrentCellDirty)
                        {
                            dataGridViewMain.CommitEdit(DataGridViewDataErrorContexts.Commit);
                        }

                        toolStripButtonDel.Enabled = true;
                        toolStripButtonDelAll.Enabled = true;
                    }
                    else
                    {
                        MessageBox.Show(Properties.Resources.Msg_InvalidFile, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void toolStripButtonDel_Click(object sender, EventArgs e)
        {
            try
            {
                foreach (DataGridViewCell cell in dataGridViewMain.SelectedCells)
                {
                    DataGridViewRow row = dataGridViewMain.Rows[cell.RowIndex];

                    if (MessageBox.Show(Properties.Resources.Msg_DelAsk, this.Text, MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
                    {
                        if (!AppManager.isRunning(row.Index))
                        {
                            dataGridViewMain.Rows.RemoveAt(row.Index);

                            if (dataGridViewMain.Rows.Count == 0)
                            {
                                toolStripButtonDel.Enabled = false;
                                toolStripButtonDelAll.Enabled = false;
                            }
                        }
                        else
                        {
                            MessageBox.Show(Properties.Resources.Msg_DelError, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void toolStripButtonDelAll_Click(object sender, EventArgs e)
        {
            try
            {
                if (MessageBox.Show(Properties.Resources.Msg_DelAsk2, this.Text, MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
                {
                    if (!AppManager.AnyRunning())
                    {
                        dataGridViewMain.Rows.Clear();

                        toolStripButtonDel.Enabled = false;
                        toolStripButtonDelAll.Enabled = false;
                    }
                    else
                    {
                        MessageBox.Show(Properties.Resources.Msg_DelError3, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void toolStripButtonRun_Click(object sender, EventArgs e)
        {
            try
            {
                if (!AppManager.AnyRunning())
                {
                    toolStripButtonEnd.Visible = true;
                    toolStripButtonRun.Visible = false;
                    toolStripButtonAdd.Enabled = false;
                    toolStripButtonDel.Enabled = false;
                    toolStripButtonDelAll.Enabled = false;

                    if (windowStyleOnRun == 0)
                    {
                        toolStripButtonShow.Visible = true;
                    }
                    else
                    {
                        toolStripButtonHide.Visible = true;
                    }

                    if (dataGridViewMain.IsCurrentCellDirty)
                    {
                        dataGridViewMain.CommitEdit(DataGridViewDataErrorContexts.Commit);
                    }

                    foreach (DataGridViewRow row in dataGridViewMain.Rows)
                    {
                        if (AppManager.isNull(row.Index))
                        {
                            if (Convert.ToBoolean(row.Cells[0].Value))
                            {
                                row.ReadOnly = true;
                                AppManager.Run(row.Index, row.Cells[3].Value.ToString(), row.Cells[4].Value.ToString(), Convert.ToInt32(row.Cells[2].Value), windowStyleOnRun);
                            }
                        }
                    }

                    timerMain.Start();

                    timerMainGrid.Start();
                    timerMainGrid_Tick(sender, e);

                    WindowState = FormWindowState.Normal;
                    Activate();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void toolStripButtonRunSelected_Click(object sender, EventArgs e)
        {
            try
            {
                if (dataGridViewMain.IsCurrentCellDirty)
                {
                    dataGridViewMain.CommitEdit(DataGridViewDataErrorContexts.Commit);
                }

                toolStripButtonEnd.Visible = true;
                toolStripButtonRun.Visible = false;

                toolStripButtonAdd.Enabled = false;
                toolStripButtonDel.Enabled = false;
                toolStripButtonDelAll.Enabled = false;

                toolStripButtonHide.Visible = true;

                foreach (DataGridViewCell cell in dataGridViewMain.SelectedCells)
                {
                    DataGridViewRow row = dataGridViewMain.Rows[cell.RowIndex];

                    if (Convert.ToBoolean(row.Cells[0].Value))
                    {
                        row.ReadOnly = true;
                        AppManager.Run(row.Index, row.Cells[3].Value.ToString(), row.Cells[4].Value.ToString(), Convert.ToInt32(row.Cells[2].Value), windowStyleOnRun);
                    }
                }

                timerMain.Start();
                timerMainGrid.Start();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void toolStripButtonEnd_Click(object sender, EventArgs e)
        {
            try
            {
                if (dataGridViewMain.IsCurrentCellDirty)
                {
                    dataGridViewMain.CommitEdit(DataGridViewDataErrorContexts.Commit);
                }

                timerMain.Stop();
                timerMainGrid.Stop();

                AppManager.KillAll();

                toolStripButtonEnd.Visible = false;
                toolStripButtonRun.Visible = true;

                toolStripButtonAdd.Enabled = true;
                toolStripButtonDel.Enabled = true;
                toolStripButtonDelAll.Enabled = true;

                toolStripButtonShow.Visible = false;
                toolStripButtonHide.Visible = false;

                foreach (DataGridViewRow row in dataGridViewMain.Rows)
                {
                    row.ReadOnly = false;
                    row.Cells[1].Value = Properties.Resources.off;
                }

                if (!AppManager.AnyRunning())
                {
                    toolStripButtonHide.Visible = false;
                    toolStripButtonShow.Visible = false;
                    statusStripMain.Items[0].Text = " not running..";
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void toolStripButtonEndSelected_Click(object sender, EventArgs e)
        {
            try
            {
                foreach (DataGridViewCell cell in dataGridViewMain.SelectedCells)
                {
                    if (AppManager.Kill(cell.RowIndex))
                    {
                        DataGridViewRow row = dataGridViewMain.Rows[cell.RowIndex];

                        row.ReadOnly = false;
                        row.Cells[1].Value = Properties.Resources.off;
                    }
                }

                if (!AppManager.AnyRunning())
                {
                    toolStripButtonHide.Visible = false;
                    toolStripButtonShow.Visible = false;

                    toolStripButtonAdd.Enabled = true;
                    toolStripButtonDel.Enabled = true;
                    toolStripButtonDelAll.Enabled = true;

                    toolStripButtonRun.Visible = true;
                    toolStripButtonEnd.Visible = false;

                    statusStripMain.Items[0].Text = " not running..";
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void FormMain_Resize(object sender, EventArgs e)
        {
            if (WindowState == FormWindowState.Minimized)
            {
                Hide();

                notifyIconMain.Visible = true;
                notifyIconMain.BalloonTipTitle = Properties.Resources.Msg_Minimized;
                notifyIconMain.BalloonTipText = Properties.Resources.Msg_MinimizedText;
                notifyIconMain.ShowBalloonTip(10000, Properties.Resources.Msg_Minimized, Properties.Resources.Msg_MinimizedText, ToolTipIcon.Info);
            }
            else if (WindowState == FormWindowState.Normal)
            {
                Show();
            }
        }

        private void notifyIconMain_DoubleClick(object sender, EventArgs e)
        {
            Show();
            WindowState = FormWindowState.Normal;
            notifyIconMain.Visible = false;
        }

        private void toolStripButtonHide_Click(object sender, EventArgs e)
        {
            if (AppManager.AnyRunning())
            {
                if (AppManager.HideAllWindows())
                {
                    toolStripButtonHide.Visible = false;
                    toolStripButtonShow.Visible = true;
                }
            }
        }

        private void toolStripButtonShow_Click(object sender, EventArgs e)
        {
            if (AppManager.AnyRunning())
            {
                if (AppManager.ShowAllWindows())
                {
                    toolStripButtonShow.Visible = false;
                    toolStripButtonHide.Visible = true;
                }
            }
        }

        private void dataGridViewMain_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            try
            {
                if (e.RowIndex >= 0 && e.ColumnIndex == 3 && !dataGridViewMain.Rows[e.RowIndex].ReadOnly)
                {
                    if (openFileDialogMain.ShowDialog() == DialogResult.OK)
                    {
                        string selectedFilePath = openFileDialogMain.FileName;

                        if (File.Exists(selectedFilePath))
                        {
                            dataGridViewMain.Rows[e.RowIndex].Cells[e.ColumnIndex].Value = selectedFilePath;

                            if (dataGridViewMain.IsCurrentCellDirty)
                            {
                                dataGridViewMain.CommitEdit(DataGridViewDataErrorContexts.Commit);
                            }

                            toolStripButtonDel.Enabled = true;
                            toolStripButtonDelAll.Enabled = true;
                        }
                        else
                        {
                            MessageBox.Show(Properties.Resources.Msg_InvalidFile, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void contextMenuStripMain_Opened(object sender, EventArgs e)
        {
            try
            {
                if (dataGridViewMain.IsCurrentCellDirty)
                {
                    dataGridViewMain.CommitEdit(DataGridViewDataErrorContexts.Commit);
                }

                bool anyRunning = false;
                bool allRunning = true;

                foreach (DataGridViewCell cell in dataGridViewMain.SelectedCells)
                {
                    bool isRunning = AppManager.isRunning(cell.RowIndex);
                    anyRunning |= isRunning;
                    allRunning &= isRunning;
                }

                contextMenuStripMain.Items[0].Visible = !anyRunning;
                contextMenuStripMain.Items[1].Visible = allRunning;
                contextMenuStripMain.Items[2].Visible = allRunning;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void toolStripMenuItemShowSelected_Click(object sender, EventArgs e)
        {
            try
            {
                if (dataGridViewMain.IsCurrentCellDirty)
                {
                    dataGridViewMain.CommitEdit(DataGridViewDataErrorContexts.Commit);
                }

                foreach (DataGridViewCell cell in dataGridViewMain.SelectedCells)
                {
                    if (AppManager.isRunning(cell.RowIndex))
                    {
                        AppManager.ShowSelectedWindow(cell.RowIndex, 1);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void toolStripMenuItemHideSelected_Click(object sender, EventArgs e)
        {
            try
            {
                if (dataGridViewMain.IsCurrentCellDirty)
                {
                    dataGridViewMain.CommitEdit(DataGridViewDataErrorContexts.Commit);
                }

                foreach (DataGridViewCell cell in dataGridViewMain.SelectedCells)
                {
                    if (AppManager.isRunning(cell.RowIndex))
                    {
                        AppManager.ShowSelectedWindow(cell.RowIndex, 0);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void optionsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (!AppManager.AnyRunning())
                {
                    using (FormSettings formSettings = new FormSettings())
                    {
                        formSettings.comboBoxWindowStyle.SelectedIndex = windowStyleOnRun;
                        formSettings.numericUpDownCheckTime.Value = processCheckTime;

                        if (formSettings.ShowDialog() == DialogResult.OK)
                        {
                            windowStyleOnRun = formSettings.comboBoxWindowStyle.SelectedIndex;
                            processCheckTime = Convert.ToInt32(formSettings.numericUpDownCheckTime.Value);

                            DataManager.SetRegSetting("windowStyleOnRun", windowStyleOnRun);
                            DataManager.SetRegSetting("processCheckTime", processCheckTime);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (var aboutForm = new Form())
            {
                aboutForm.Text = "About";
                aboutForm.Size = new Size(300, 180);
                aboutForm.FormBorderStyle = FormBorderStyle.FixedDialog;
                aboutForm.MaximizeBox = false;
                aboutForm.MinimizeBox = false;
                aboutForm.StartPosition = FormStartPosition.CenterParent;

                var labelsFont = new Font(FontFamily.GenericSansSerif, 12);

                var nameLabel = new Label
                {
                    Text = "File Name: ServerStartUp",
                    Dock = DockStyle.Top,
                    TextAlign = ContentAlignment.MiddleCenter,
                    Font = labelsFont,
                };

                var releaseLabel = new Label
                {
                    Text = $"Version: {System.Reflection.Assembly.GetExecutingAssembly().GetName().Version}",
                    Dock = DockStyle.Top,
                    TextAlign = ContentAlignment.MiddleCenter,
                    Font = labelsFont,
                };

                var authorLabel = new Label
                {
                    Text = "By: Qubit",
                    Dock = DockStyle.Top,
                    TextAlign = ContentAlignment.MiddleCenter,
                    Font = labelsFont,
                };

                var okButton = new Button
                {
                    Text = "OK",
                    DialogResult = DialogResult.OK,
                    Size = new Size(80, 30),
                    FlatStyle = FlatStyle.Flat,
                    BackColor = Color.FromArgb(0, 123, 255),
                    ForeColor = Color.White,
                    Font = new Font(FontFamily.GenericSansSerif, 10, FontStyle.Bold),
                    Dock = DockStyle.Bottom,
                };

                aboutForm.Controls.AddRange(new Control[] { nameLabel, releaseLabel, authorLabel, okButton });

                aboutForm.AcceptButton = okButton;

                aboutForm.ShowDialog();
            }
        }

        private void menuStripMain_MenuActivate(object sender, EventArgs e)
        {
            optionsToolStripMenuItem.Enabled = !AppManager.AnyRunning();
        }
    }
}
