using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;

namespace RelayControl2010
{
    public partial class Form1 : Form
    {
        string[] ports = SerialPort.GetPortNames();
        public Form1()
        {
            InitializeComponent();
            comboBox1.Enabled = false;
            button1.Enabled = false;
            groupBox1.Enabled = false;
            groupBox2.Enabled = false;
            trackBar1.Minimum = 0;
            trackBar1.Maximum = 2000;
            trackBar1.Value = 0;
            trackBar1.TickFrequency = 100;
            comboBox2.Items.AddRange(new object[] {1, 5, 10, 50, 100, 500});
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (button1.Text == "Connect")
            {
                serialPort1.BaudRate = 9600;
                serialPort1.DataBits = 8;
                serialPort1.Parity = System.IO.Ports.Parity.None;
                serialPort1.StopBits = System.IO.Ports.StopBits.One;
                serialPort1.Handshake = System.IO.Ports.Handshake.None;
                serialPort1.ReadTimeout = 500;
                serialPort1.WriteTimeout = 500;
                serialPort1.Open();
                groupBox1.Enabled = true;
                groupBox2.Enabled = true;
                button1.Text = "Disconnect";

            }
            else
            {
                serialPort1.Close();
                button1.Text = "Connect";
                groupBox1.Enabled = false;
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox checkBox = (CheckBox)sender;
            if (checkBox.Checked == true)
            {
                serialPort1.WriteLine("@1 ON$");
            }
            else
            {
                serialPort1.WriteLine("@1 OFF$");
            }
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox checkBox = (CheckBox)sender;
            if (checkBox.Checked == true)
            {
                serialPort1.WriteLine("@2 ON$");
            }
            else
            {
                serialPort1.WriteLine("@2 OFF$");
            }
        }

        private void checkBox3_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox checkBox = (CheckBox)sender;
            if (checkBox.Checked == true)
            {
                serialPort1.WriteLine("@3 ON$");
            }
            else
            {
                serialPort1.WriteLine("@3 OFF$");
            }
        }

        private void checkBox4_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox checkBox = (CheckBox)sender;
            if (checkBox.Checked == true)
            {
                serialPort1.WriteLine("@4 ON$");
            }
            else
            {
                serialPort1.WriteLine("@4 OFF$");
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            ports = SerialPort.GetPortNames();
            comboBox1.Items.Clear();
            comboBox1.Items.AddRange(ports);
            comboBox1.Enabled = true;
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            serialPort1.PortName = comboBox1.SelectedItem.ToString();
            button1.Enabled = true;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            serialPort1.WriteLine("@SETUP#0$");
        }

        private void button4_Click(object sender, EventArgs e)
        {
            serialPort1.WriteLine("@CLEAR#0$");
        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            label3.Text = trackBar1.Value.ToString();
        }

        private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
        {
            trackBar1.SmallChange = int.Parse(comboBox2.SelectedItem.ToString());
        }

        private void button5_Click(object sender, EventArgs e)
        {
            int freq = trackBar1.Value;
            string message = "@SET#";
            message += freq.ToString() + "$";
            serialPort1.WriteLine(message);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            string answer = "";
            for (int i = 0; i < 10; i++)
            {
                if (serialPort1.BytesToRead == 0)
                {
                    Thread.Sleep(100);
                    answer = "Empty.";
                }
                else
                {
                    answer = "";
                    break;
                }
            }
            if (answer.Equals(""))
            {
                for (int j = serialPort1.BytesToRead; j > 0; j--)
                {
                    answer += (char)serialPort1.ReadByte();
                }
            }
            richTextBox1.Text = answer;
        }

        private void button7_Click(object sender, EventArgs e)
        {
            richTextBox1.Text = "";
        }
    }
}
