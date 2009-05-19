/*
	Aseba - an event-based framework for distributed robot control
	Copyright (C) 2007--2009:
		Stephane Magnenat <stephane at magnenat dot net>
		(http://stephane.magnenat.net)
		and other contributors, see authors.txt for details
		Mobots group, Laboratory of Robotics Systems, EPFL, Lausanne
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any other version as decided by the two original authors
	Stephane Magnenat and Valentin Longchamp.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "EventViewer.h"
#include "../common/types.h"
#include <vector>
#include <QtDebug>

#ifdef HAVE_QWT

namespace Aseba
{
	/** \addtogroup studio */
	/*@{*/
	
	class EventDataWrapper : public QwtData
	{
	private:
		std::vector<double>& _x;
		std::vector<sint16>& _y;
		
	public:
		EventDataWrapper(std::vector<double>& _x, std::vector<sint16>& _y) :
			_x(_x),
			_y(_y)
		{ }
		virtual QwtData *   copy () const { return new EventDataWrapper(*this); }
		virtual size_t   size () const { return _x.size(); }
		virtual double x (size_t i) const { return _x[i]; }
		virtual double y (size_t i) const { return (double)_y[i]; }
	};
	
	EventViewer::EventViewer(unsigned eventId, const QString& eventName, unsigned eventVariablesCount, MainWindow::EventViewers* eventsViewers) :
		QwtPlot(QwtText(QString(tr("Plot for event %1 (number %2)")).arg(eventName).arg(eventId))),
		eventId(eventId),
		eventsViewers(eventsViewers),
		values(eventVariablesCount),
		startingTime(QTime::currentTime())
	{
		setCanvasBackground(Qt::white);
		setAxisTitle(xBottom, tr("Time (seconds)"));
		setAxisTitle(yLeft, tr("Values"));
		
		QwtLegend *legend = new QwtLegend;
		//legend->setItemMode(QwtLegend::CheckableItem);
		insertLegend(legend, QwtPlot::BottomLegend);
		
		for (size_t i = 0; i < values.size(); i++)
		{
			QwtPlotCurve *curve = new QwtPlotCurve(QString("%0").arg(i));
			curve->setData(EventDataWrapper(timeStamps, values[i]));
			curve->attach(this);
			curve->setPen(QColor::fromHsv((i * 360) / values.size(), 255, 100));
		}
		
		eventsViewers->insert(eventId, this);
	}
	
	EventViewer::~EventViewer()
	{
		eventsViewers->remove(eventId, this);
	}
	
	void EventViewer::addData(const VariablesDataVector& data)
	{
		double elapsedTime = (double)startingTime.msecsTo(QTime::currentTime()) / 1000.;
		timeStamps.push_back(elapsedTime);
		for (size_t i = 0; i < values.size(); i++)
		{
			if (i < data.size())
			{
				values[i].push_back(data[i]);
			}
			else
			{
				values[i].push_back(0);
			}
		}
		replot();
	}
	
	/*@}*/
}; // Aseba

#endif // HAVE_QWT