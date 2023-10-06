using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Xml.Linq;

namespace ServerStartUp
{
    class DataMng
    {
        public bool SetRegSetting(string Setting, object Value)
        {
            try
            {
                RegistryKey keyHandle = Registry.CurrentUser.CreateSubKey(@"SOFTWARE\ServerStartUp");

                keyHandle.SetValue(Setting, Value);
                keyHandle.Close();

                return true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }

            return false;
        }

        public object GetRegSetting(string Setting)
        {
            try
            {
                RegistryKey keyHandle = Registry.CurrentUser.CreateSubKey(@"SOFTWARE\ServerStartUp");

                var Result = keyHandle.GetValue(Setting);
                keyHandle.Close();

                return Result;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }

            return null;
        }

        public object LoadRegSetting(string Setting, object DefaultValue)
        {
            object Result = GetRegSetting(Setting);

            if (Result == null)
            {
                SetRegSetting(Setting, DefaultValue);
                return DefaultValue;
            }

            return Result;
        }

        public void SaveDataToXml(DataGridView dataGrid)
        {
            XDocument xmlDocument = new XDocument(new XElement("StartUp"));

            foreach (DataGridViewRow row in dataGrid.Rows)
            {
                xmlDocument.Root.Add(
                    new XElement(
                        "Process",
                        new XAttribute("Run", row.Cells[0].Value.ToString()),
                        new XAttribute("Delay", row.Cells[2].Value.ToString()),
                        new XAttribute("Path", row.Cells[3].Value.ToString()),
                        new XAttribute("Parameters", row.Cells[4].Value.ToString())
                    )
                );
            }

            xmlDocument.Save(Properties.Resources.Msg_FileName);
        }

        public void LoadDataFromXml()
        {
            FormMain Form = (FormMain)Application.OpenForms[0];

            try
            {
                if (File.Exists(Properties.Resources.Msg_FileName))
                {
                    XDocument xmlDocument = XDocument.Load(Properties.Resources.Msg_FileName);

                    foreach (XElement el in xmlDocument.Root.Elements())
                    {
                        if(el.Name.LocalName == "Process")
                        {
                            if (File.Exists(el.Attribute("Path").Value))
                            {
                                Form.dataGridViewMain.Rows.Add
                                (
                                    el.Attribute("Run").Value,
                                    Properties.Resources.off,
                                    el.Attribute("Delay").Value,
                                    el.Attribute("Path").Value,
                                    el.Attribute("Parameters").Value
                                );
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }
    }
}
