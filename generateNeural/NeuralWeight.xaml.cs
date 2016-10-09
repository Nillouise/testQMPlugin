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
using System.Windows.Shapes;
using System.Data;
using System.Data.SqlClient;
namespace generateNeural
{
    /// <summary>
    /// Interaction logic for NeuralWeight.xaml
    /// </summary>
    public partial class NeuralWeight : Window
    {
        public NeuralWeight()
        {
            InitializeComponent();
        }

        static DataSet getNeural()
        {
            string strConnection = @"Data Source = (LocalDB)\MSSQLLocalDB; AttachDbFilename = "+@"C:\Users\win7x64\Documents\Visual Studio 2015\Projects\testQMPlugin\Neural.mdf"+@"; Integrated Security = True; Connect Timeout = 30";

            DataSet dataSet;
            bool CanConnectDB = false;
            using (SqlConnection objConnection = new SqlConnection(strConnection))
            {
                try
                {
                    objConnection.Open();
                    CanConnectDB = true;
                    objConnection.Close();
                }
                catch { }
                if (CanConnectDB)
                {
                    dataSet = new DataSet();
                    SqlDataAdapter dataAdapter = new SqlDataAdapter(@"select * from Neural", objConnection);
                    dataAdapter.Fill(dataSet);

                }
                else
                {
                    return null;
                }
            }
            return dataSet;
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {
            DataSet data = getNeural();
            foreach (DataRow mDr in data.Tables[0].Rows)
            {
                foreach (DataColumn mDc in data.Tables[0].Columns)
                {
                    Console.WriteLine(mDr[mDc].ToString());
                }
            }
        }
    }
}
