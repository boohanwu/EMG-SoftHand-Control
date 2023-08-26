using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using System.Net.Sockets;

namespace EMG_gui
{
    public partial class Form1 : Form
    {
        Socket socket;
        public int classID = 0;
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            label0.Text = "Class 0: Relax";
            label1.Text = "Class 1: Hold";
            label2.Text = "Class 2: Open";
            label3.Text = "Class 3: Two";
            label4.Text = "Class 4: Three";
            label5.Text = "Class 5: Four";
            label6.Text = "Class 6: Six";
            label7.Text = "Class 7: Seven";

            pictureBox0.Image = Image.FromFile("Relax.jpg");
            Image img1 = pictureBox0.Image;
            img1.RotateFlip(RotateFlipType.Rotate90FlipNone);
            pictureBox0.Image = img1;

            pictureBox1.Image = Image.FromFile("Hold.jpg");
            Image img2 = pictureBox1.Image;
            img2.RotateFlip(RotateFlipType.Rotate90FlipNone);
            pictureBox1.Image = img2;

            pictureBox2.Image = Image.FromFile("Open.jpg");
            Image img3 = pictureBox2.Image;
            img3.RotateFlip(RotateFlipType.Rotate90FlipNone);
            pictureBox2.Image = img3;

            pictureBox3.Image = Image.FromFile("Two.jpg");
            Image img4 = pictureBox3.Image;
            img4.RotateFlip(RotateFlipType.Rotate90FlipNone);
            pictureBox3.Image = img4;

            pictureBox4.Image = Image.FromFile("Three.jpg");
            Image img5 = pictureBox4.Image;
            img5.RotateFlip(RotateFlipType.Rotate90FlipNone);
            pictureBox4.Image = img5;

            pictureBox5.Image = Image.FromFile("Four.jpg");
            Image img6 = pictureBox5.Image;
            img6.RotateFlip(RotateFlipType.Rotate90FlipNone);
            pictureBox5.Image = img6;

            pictureBox6.Image = Image.FromFile("Six.jpg");
            Image img7 = pictureBox6.Image;
            img7.RotateFlip(RotateFlipType.Rotate90FlipNone);
            pictureBox6.Image = img7;

            pictureBox7.Image = Image.FromFile("Seven.jpg");
            Image img8 = pictureBox7.Image;
            img8.RotateFlip(RotateFlipType.Rotate90FlipNone);
            pictureBox7.Image = img8;
        }

        private void btn_start_Click(object sender, EventArgs e)
        {
            try
            {
                socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                socket.Connect("127.0.0.1", 36000);
                Console.WriteLine("Connect successful ! \n");
                Thread t1 = new Thread(new ThreadStart(Read_Data));
                t1.Start();
            }
            catch
            {
                Console.WriteLine("server: " + "127.0.0.1" + "port: 8000 can't connect \r\n");
                return;
            }
        }
        void Read_Data()
        {
            byte[] myBufferBytes = new byte[1024];
            while (true)
            {
                try
                {
                    int dataLengh;

                    dataLengh = socket.Receive(myBufferBytes);
                    Array.Resize(ref myBufferBytes, dataLengh);
                    //BeginInvoke(new MethodInvoker(() =>
                    //{
                    if (dataLengh > 0)
                    {
                        Console.WriteLine(myBufferBytes[0]);
                        //socket.Send(myBufferBytes, dataLenght,0);
                    }
                }
                catch (SocketException ex)
                {
                    MessageBox.Show(ex.ToString());
                }

                classID = myBufferBytes[0] - 48;    // ASCII code transfer to the real number.

                Graphics Graphics1 = pictureBox0.CreateGraphics();
                Graphics Graphics2 = pictureBox1.CreateGraphics();
                Graphics Graphics3 = pictureBox2.CreateGraphics();
                Graphics Graphics4 = pictureBox3.CreateGraphics();
                Graphics Graphics5 = pictureBox4.CreateGraphics();
                Graphics Graphics6 = pictureBox5.CreateGraphics();
                Graphics Graphics7 = pictureBox6.CreateGraphics();
                Graphics Graphics8 = pictureBox7.CreateGraphics();


                if (classID == 1)
                {
                    Graphics1.DrawRectangle(new Pen(Color.Red, 10), 0, 0, pictureBox0.Width - 1, pictureBox0.Height - 1);
                    Graphics2.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox1.Width - 1, pictureBox1.Height - 1);
                    Graphics3.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox2.Width - 1, pictureBox2.Height - 1);
                    Graphics4.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox3.Width - 1, pictureBox3.Height - 1);
                    Graphics5.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox4.Width - 1, pictureBox4.Height - 1);
                    Graphics6.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox5.Width - 1, pictureBox5.Height - 1);
                    Graphics7.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox6.Width - 1, pictureBox6.Height - 1);
                    Graphics8.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox7.Width - 1, pictureBox7.Height - 1);
                }
                else if (classID == 2)
                {
                    Graphics1.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox0.Width - 1, pictureBox0.Height - 1);
                    Graphics2.DrawRectangle(new Pen(Color.Red, 10), 0, 0, pictureBox1.Width - 1, pictureBox1.Height - 1);
                    Graphics3.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox2.Width - 1, pictureBox2.Height - 1);
                    Graphics4.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox3.Width - 1, pictureBox3.Height - 1);
                    Graphics5.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox4.Width - 1, pictureBox4.Height - 1);
                    Graphics6.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox5.Width - 1, pictureBox5.Height - 1);
                    Graphics7.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox6.Width - 1, pictureBox6.Height - 1);
                    Graphics8.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox7.Width - 1, pictureBox7.Height - 1);
                }
                else if (classID == 3)
                {
                    Graphics1.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox0.Width - 1, pictureBox0.Height - 1);
                    Graphics2.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox1.Width - 1, pictureBox1.Height - 1);
                    Graphics3.DrawRectangle(new Pen(Color.Red, 10), 0, 0, pictureBox2.Width - 1, pictureBox2.Height - 1);
                    Graphics4.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox3.Width - 1, pictureBox3.Height - 1);
                    Graphics5.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox4.Width - 1, pictureBox4.Height - 1);
                    Graphics6.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox5.Width - 1, pictureBox5.Height - 1);
                    Graphics7.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox6.Width - 1, pictureBox6.Height - 1);
                    Graphics8.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox7.Width - 1, pictureBox7.Height - 1);
                }
                else if (classID == 4)
                {
                    Graphics1.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox0.Width - 1, pictureBox0.Height - 1);
                    Graphics2.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox1.Width - 1, pictureBox1.Height - 1);
                    Graphics3.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox2.Width - 1, pictureBox2.Height - 1);
                    Graphics4.DrawRectangle(new Pen(Color.Red, 10), 0, 0, pictureBox3.Width - 1, pictureBox3.Height - 1);
                    Graphics5.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox4.Width - 1, pictureBox4.Height - 1);
                    Graphics6.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox5.Width - 1, pictureBox5.Height - 1);
                    Graphics7.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox6.Width - 1, pictureBox6.Height - 1);
                    Graphics8.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox7.Width - 1, pictureBox7.Height - 1);
                }
                else if (classID == 5)
                {
                    Graphics1.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox0.Width - 1, pictureBox0.Height - 1);
                    Graphics2.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox1.Width - 1, pictureBox1.Height - 1);
                    Graphics3.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox2.Width - 1, pictureBox2.Height - 1);
                    Graphics4.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox3.Width - 1, pictureBox3.Height - 1);
                    Graphics5.DrawRectangle(new Pen(Color.Red, 10), 0, 0, pictureBox4.Width - 1, pictureBox4.Height - 1);
                    Graphics6.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox5.Width - 1, pictureBox5.Height - 1);
                    Graphics7.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox6.Width - 1, pictureBox6.Height - 1);
                    Graphics8.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox7.Width - 1, pictureBox7.Height - 1);
                }
                else if (classID == 6)
                {
                    Graphics1.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox0.Width - 1, pictureBox0.Height - 1);
                    Graphics2.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox1.Width - 1, pictureBox1.Height - 1);
                    Graphics3.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox2.Width - 1, pictureBox2.Height - 1);
                    Graphics4.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox3.Width - 1, pictureBox3.Height - 1);
                    Graphics5.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox4.Width - 1, pictureBox4.Height - 1);
                    Graphics6.DrawRectangle(new Pen(Color.Red, 10), 0, 0, pictureBox5.Width - 1, pictureBox5.Height - 1);
                    Graphics7.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox6.Width - 1, pictureBox6.Height - 1);
                    Graphics8.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox7.Width - 1, pictureBox7.Height - 1);
                }
                else if (classID == 7)
                {
                    Graphics1.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox0.Width - 1, pictureBox0.Height - 1);
                    Graphics2.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox1.Width - 1, pictureBox1.Height - 1);
                    Graphics3.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox2.Width - 1, pictureBox2.Height - 1);
                    Graphics4.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox3.Width - 1, pictureBox3.Height - 1);
                    Graphics5.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox4.Width - 1, pictureBox4.Height - 1);
                    Graphics6.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox5.Width - 1, pictureBox5.Height - 1);
                    Graphics7.DrawRectangle(new Pen(Color.Red, 10), 0, 0, pictureBox6.Width - 1, pictureBox6.Height - 1);
                    Graphics8.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox7.Width - 1, pictureBox7.Height - 1);
                }
                else if (classID == 8)
                {
                    Graphics1.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox0.Width - 1, pictureBox0.Height - 1);
                    Graphics2.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox1.Width - 1, pictureBox1.Height - 1);
                    Graphics3.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox2.Width - 1, pictureBox2.Height - 1);
                    Graphics4.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox3.Width - 1, pictureBox3.Height - 1);
                    Graphics5.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox4.Width - 1, pictureBox4.Height - 1);
                    Graphics6.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox5.Width - 1, pictureBox5.Height - 1);
                    Graphics7.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox6.Width - 1, pictureBox6.Height - 1);
                    Graphics8.DrawRectangle(new Pen(Color.Red, 10), 0, 0, pictureBox7.Width - 1, pictureBox7.Height - 1);
                }
                else
                {
                    Graphics1.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox0.Width - 1, pictureBox0.Height - 1);
                    Graphics2.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox1.Width - 1, pictureBox1.Height - 1);
                    Graphics3.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox2.Width - 1, pictureBox2.Height - 1);
                    Graphics4.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox3.Width - 1, pictureBox3.Height - 1);
                    Graphics5.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox4.Width - 1, pictureBox4.Height - 1);
                    Graphics6.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox5.Width - 1, pictureBox5.Height - 1);
                    Graphics7.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox6.Width - 1, pictureBox6.Height - 1);
                    Graphics8.DrawRectangle(new Pen(Color.White, 10), 0, 0, pictureBox7.Width - 1, pictureBox7.Height - 1);
                }
            }
        }
    }
}
