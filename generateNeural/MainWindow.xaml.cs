using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Neural;
using System.Data;
using System.Data.SqlClient;
namespace generateNeural
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {
            string[] newNames = txt_ClassName.Text.Split(new string[] { "\r\n" }, StringSplitOptions.None);
            string outputcode = Neural.generateCode.generateMulText(txt_replaceText.Text, txt_replaceTextName.Text, newNames);

            txt_OuputCode.Text = outputcode;
        }

        private void btn_DataBase_Click(object sender, RoutedEventArgs e)
        {
            NeuralWeight baseWindow =  new NeuralWeight();
            baseWindow.Show();
        }
    }
}
