/*
 * Copyright (C) 2011 Georgia Institute of Technology, University of Utah,
 * Weill Cornell Medical College
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This is a template implementation file for a user module derived from
 * DefaultGUIModel with a custom GUI.
 */

#include "quantizer.h"
#include <iostream>
#include <main_window.h>
#include <cmath>

extern "C" Plugin::Object*
createRTXIPlugin(void)
{
  return new Quantizer();
}

static DefaultGUIModel::variable_t vars[] = {
  {
    "Range", "g in y = g*(x + b)",
    DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
  },
  {
    "Bias", "b in y = g*(x + b)",
    DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
  },
  {
    "NLevels", "b in y = clip( round(g*(x + b)) ,0, nlevels)",
    DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
  },

  {
    "x-in", "x in y = g*(x + b)",
    DefaultGUIModel::INPUT,
  },

  {
    "y-out", "y in y = clip( round(g*x + b) ,0, nlevels)",
    DefaultGUIModel::OUTPUT ,
  },
    {
    "x-map", "x_map = g*(x+b)",
    DefaultGUIModel::OUTPUT ,
  },
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

Quantizer::Quantizer(void)
  : DefaultGUIModel("Quantizer with Custom GUI", ::vars, ::num_vars)
{
  setWhatsThis("<p><b>Quantizer:</b><br>QWhatsThis description.</p>");
  DefaultGUIModel::createGUI(vars,
                             num_vars); // this is required to create the GUI
  customizeGUI();
  initParameters();
  update(INIT); // this is optional, you may place initialization code directly
                // into the constructor
  refresh();    // this is required to update the GUI with parameter and state
                // values
  QTimer::singleShot(0, this, SLOT(resizeMe()));
}

Quantizer::~Quantizer(void)
{
}

void
Quantizer::execute(void)
{
   x_in= input(0);


   std::rotate(inputBuffer.begin(), inputBuffer.begin() + 1, inputBuffer.end());
    inputBuffer[0] = x_in;
   // inputBuffer[0] = x_in;
   //inputBuffer.assign(1,x_in);

  gain = static_cast<double>(numLevels)/range;
  x_map =gain*(x_in+bias);
  y_quant = floor(x_map);
  
  if (y_quant<0)
  {
    y_quant=0;
  }
  if (y_quant>(numLevels-1))
  {
    y_quant = numLevels-1;
  }
  
  output(0) = y_quant;
  output(1) = x_map;
  return;
}

void
Quantizer::initParameters(void)
{
    gain = 1;
    range = 2;
    bias = 0;
    numLevels = 2;
    buffLen=1e3;
    
  for (int i=1;i<buffLen;i++)
  {
   inputBuffer.push_back(i);
   }
}

void
Quantizer::update(DefaultGUIModel::update_flags_t flag)
{
  switch (flag) {
    case INIT:
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      
      
        setParameter("Range", range);
        //setParameter("Gain", gain);
        setParameter("Bias", bias);
        setParameter("NLevels", numLevels);
      
      break;

    case MODIFY:
      some_parameter = getParameter("GUI label").toDouble();
     // gain = getParameter("Gain").toDouble();
      range = getParameter("Range").toDouble();
	  bias = getParameter("Bias").toDouble();
	  numLevels = getParameter("NLevels").toDouble();
      break;

    case UNPAUSE:
      break;

    case PAUSE:
      break;

    case PERIOD:
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      break;

    default:
      break;
  }
}

void
Quantizer::customizeGUI(void)
{
  QGridLayout* customlayout = DefaultGUIModel::getLayout();

  QGroupBox* button_group = new QGroupBox;

  QPushButton* abutton = new QPushButton("Auto-calibrate");
  QPushButton* bbutton = new QPushButton("Button B");
  QHBoxLayout* button_layout = new QHBoxLayout;
  button_group->setLayout(button_layout);
  button_layout->addWidget(abutton);
  button_layout->addWidget(bbutton);
  QObject::connect(abutton, SIGNAL(clicked()), this, SLOT(aBttn_event()));
  QObject::connect(bbutton, SIGNAL(clicked()), this, SLOT(bBttn_event()));

  customlayout->addWidget(button_group, 0, 0);
  setLayout(customlayout);
}

// functions designated as Qt slots are implemented as regular C++ functions
void
Quantizer::aBttn_event(void)
{
   /*
   for (int i=0; i<buffLen; i++)
   {
   std::cout<<inputBuffer[i]<<',';
   }
   std::cout<<"\n[";
   */
   bias = *std::min_element(inputBuffer.begin(), inputBuffer.end());
   bias = -bias;
   double max = *std::max_element(inputBuffer.begin(), inputBuffer.end());
   range = max+bias;
   
   std::cout<<"max"<<max<<"min"<<-bias;
   setParameter("Range", range);
        //setParameter("Gain", gain);
    setParameter("Bias", bias);
   
}

void
Quantizer::bBttn_event(void)
{
}
