namespace ServerStartUp
{
    partial class FormSettings
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormSettings));
            this.buttonCancel = new System.Windows.Forms.Button();
            this.buttonOk = new System.Windows.Forms.Button();
            this.groupBoxSettings = new System.Windows.Forms.GroupBox();
            this.numericUpDownCheckTime = new System.Windows.Forms.NumericUpDown();
            this.labelCheckTime = new System.Windows.Forms.Label();
            this.labelWindowStyle = new System.Windows.Forms.Label();
            this.comboBoxWindowStyle = new System.Windows.Forms.ComboBox();
            this.groupBoxSettings.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownCheckTime)).BeginInit();
            this.SuspendLayout();
            // 
            // buttonCancel
            // 
            this.buttonCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonCancel.Location = new System.Drawing.Point(112, 107);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(75, 23);
            this.buttonCancel.TabIndex = 0;
            this.buttonCancel.Text = "CANCEL";
            this.buttonCancel.UseVisualStyleBackColor = true;
            // 
            // buttonOk
            // 
            this.buttonOk.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonOk.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.buttonOk.Location = new System.Drawing.Point(9, 107);
            this.buttonOk.Name = "buttonOk";
            this.buttonOk.Size = new System.Drawing.Size(75, 23);
            this.buttonOk.TabIndex = 1;
            this.buttonOk.Text = "OK";
            this.buttonOk.UseVisualStyleBackColor = true;
            // 
            // groupBoxSettings
            // 
            this.groupBoxSettings.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxSettings.Controls.Add(this.numericUpDownCheckTime);
            this.groupBoxSettings.Controls.Add(this.labelCheckTime);
            this.groupBoxSettings.Controls.Add(this.labelWindowStyle);
            this.groupBoxSettings.Controls.Add(this.comboBoxWindowStyle);
            this.groupBoxSettings.Controls.Add(this.buttonCancel);
            this.groupBoxSettings.Controls.Add(this.buttonOk);
            this.groupBoxSettings.Location = new System.Drawing.Point(12, 12);
            this.groupBoxSettings.Name = "groupBoxSettings";
            this.groupBoxSettings.Size = new System.Drawing.Size(193, 136);
            this.groupBoxSettings.TabIndex = 2;
            this.groupBoxSettings.TabStop = false;
            // 
            // numericUpDownCheckTime
            // 
            this.numericUpDownCheckTime.BackColor = System.Drawing.SystemColors.Window;
            this.numericUpDownCheckTime.Location = new System.Drawing.Point(6, 75);
            this.numericUpDownCheckTime.Maximum = new decimal(new int[] {
            10000000,
            0,
            0,
            0});
            this.numericUpDownCheckTime.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDownCheckTime.Name = "numericUpDownCheckTime";
            this.numericUpDownCheckTime.Size = new System.Drawing.Size(177, 20);
            this.numericUpDownCheckTime.TabIndex = 7;
            this.numericUpDownCheckTime.Tag = "";
            this.numericUpDownCheckTime.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // labelCheckTime
            // 
            this.labelCheckTime.AutoSize = true;
            this.labelCheckTime.Location = new System.Drawing.Point(6, 59);
            this.labelCheckTime.Name = "labelCheckTime";
            this.labelCheckTime.Size = new System.Drawing.Size(146, 13);
            this.labelCheckTime.TabIndex = 6;
            this.labelCheckTime.Text = "Check process time (Minutes)";
            // 
            // labelWindowStyle
            // 
            this.labelWindowStyle.AutoSize = true;
            this.labelWindowStyle.Location = new System.Drawing.Point(6, 13);
            this.labelWindowStyle.Name = "labelWindowStyle";
            this.labelWindowStyle.Size = new System.Drawing.Size(126, 13);
            this.labelWindowStyle.TabIndex = 3;
            this.labelWindowStyle.Text = "Window Style on run app";
            // 
            // comboBoxWindowStyle
            // 
            this.comboBoxWindowStyle.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxWindowStyle.Location = new System.Drawing.Point(6, 29);
            this.comboBoxWindowStyle.Name = "comboBoxWindowStyle";
            this.comboBoxWindowStyle.Size = new System.Drawing.Size(177, 21);
            this.comboBoxWindowStyle.TabIndex = 2;
            // 
            // FormSettings
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(217, 160);
            this.Controls.Add(this.groupBoxSettings);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormSettings";
            this.ShowInTaskbar = false;
            this.Text = "Settings";
            this.groupBoxSettings.ResumeLayout(false);
            this.groupBoxSettings.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownCheckTime)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button buttonCancel;
        private System.Windows.Forms.Button buttonOk;
        private System.Windows.Forms.GroupBox groupBoxSettings;
        private System.Windows.Forms.Label labelWindowStyle;
        public System.Windows.Forms.ComboBox comboBoxWindowStyle;
        private System.Windows.Forms.Label labelCheckTime;
        public System.Windows.Forms.NumericUpDown numericUpDownCheckTime;
    }
}