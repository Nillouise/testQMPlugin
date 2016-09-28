﻿using System;

namespace Neural
{
    class generateCode
    {
        public static string simulation(string className, int number)
        {
            string objectName = "simu" + className;
            string rs = "";
            for (int i = 1; i <= number; i++)
            {
                string curMsg = "";
                string curObject = objectName + i.ToString();
                curMsg += className + "* " + curObject + " = new " + className + "();\r\n";
                curMsg += curObject + "->m_MonToAttack = &g_monNeural" + ((i % 2) + 1).ToString() + ";\n";
                curMsg += "g_AnyToAct[&g_action].insert(" + curObject + ");\n\n";
                rs += curMsg;
            }



            return rs;
        }
    }
}
