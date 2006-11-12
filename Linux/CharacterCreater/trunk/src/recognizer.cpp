#include "recognizer.h"

Recognizer & Recognizer::Instance()
{
	static Recognizer ret;
	return ret;
}

template <typename T>
T _max(T arg1, T arg2)
{
	return (arg1<arg2)?arg2:arg1;
}

template <typename T>
T _min(T arg1, T arg2)
{
	return (arg1<arg2)?arg1:arg2;
}

template <typename T>
T _abs(T _num)
{
	return (_num<T())?-_num:_num;
}

#include <iostream>
#include <iterator>
#include <algorithm>
// !!! FIXME !!! 
// !!! Magic Numbers !!!
static const double ANGLE_THRESHOLD = 30.0 / 180.0 * MATH_PI;	//< 30 deg
static const double DIST_THRESHOLD = 1.0 / 8.0;			//< 1/8 of the diagonal line
Stroke
Recognizer::normalize( Stroke & orig_stroke ) const
{
	Point::collection normalized;
	copy(orig_stroke.points_begin(), orig_stroke.points_end(), back_inserter(normalized));

	// find threshold;
	Point l_u(*(orig_stroke.points_begin())),
	      r_b(*(orig_stroke.points_begin()));
	for (Point::iterator pi = orig_stroke.points_begin();
	     pi != orig_stroke.points_end();
	     ++pi)
	{
		if (l_u.x() > pi->x())
			l_u.x() = pi->x();
		if (l_u.y() > pi->y())
			l_u.y() = pi->y();
		if (r_b.x() < pi->x())
			r_b.x() = pi->x();
		if (r_b.y() < pi->y())
			r_b.y() = pi->y();
	}
	int dist_threshold = (l_u - r_b).abs() * DIST_THRESHOLD;

	// get rid of useless points
	bool erased_something = true;
	while (erased_something)
	{
		erased_something = false;

		// erase segment with too-close angles
		bool erased_angle = true;
		while (erased_angle)
		{
			erased_angle = false;
			for (Point::iterator pi = normalized.begin();
			     pi != normalized.end();)
			{
				Point::iterator pf = pi, pm = pi+1, pl = pi+2;
				if (pm == normalized.end() || pl == normalized.end())
					break;
				double angle = _min(
					_abs((*pf - *pm).arg() - (*pm - *pl).arg()),
					_abs((*pf - *pm).arg() + (*pm - *pl).arg()));
				if (angle < ANGLE_THRESHOLD)
				{
					erased_something = erased_angle = true;
					pi = normalized.erase(pm);
				}
				else
					++pi;
			}
		}

		// erase segment with too-short distance
		int norm_size = normalized.size();
		for (Point::iterator pi = normalized.begin();
		     pi != normalized.end();)
		{
			Point::iterator pf = pi, pl = pi + 1;
			if (pl == normalized.end())
				break;
			if ((*pf - *pl).abs() < dist_threshold)
			{
				Point pm((pf->x() + pl->x()) / 2, (pf->y() + pl->y()) / 2);
				erased_something = true;
				*pf = pm;
				pi = normalized.erase(pl);
			}
			else
				++pi;
		}
		// snap to origional stroke
		for (Point::iterator pi = normalized.begin();
		     pi != normalized.end();
		     ++pi)
		{
			Point p(*pi);
			double min_distance = (p - *(orig_stroke.points_begin())).abs();
			for (Point::iterator ppi = orig_stroke.points_begin();
			     ppi != orig_stroke.points_end();
			     ++ppi)
			{
				if ((p - *ppi).abs() < min_distance)
				{
					min_distance = (p - *ppi).abs();
					*pi = *ppi;
				}
			}
		}
	}

	Stroke ret;
	for (Point::iterator pi = normalized.begin();
	     pi != normalized.end();
	     ++pi)
		ret.add_point(*pi);
	return ret;
}

#include <iostream>
#include <iterator>
Recognizer::STROKE_TYPES
Recognizer::recognize(Stroke & stroke) const
{
	Stroke normalized = normalize(stroke);

	double correspondence[STROKE_TYPE_SIZE] = { 0.0 };

	// a neural network (in some way...) to calculate the
	// correspondence of a stroke.
	
	double length = stroke_length(normalized.points_begin(), normalized.points_end());

	correspondence[STROKE_H] = recognize_stroke_by_radical(
		normalized.points_begin(), normalized.points_end(), length, Point(1, 0));
	correspondence[STROKE_S] = recognize_stroke_by_radical(
		normalized.points_begin(), normalized.points_end(), length, Point(0, 1));
	correspondence[STROKE_T] = recognize_stroke_by_radical(
		normalized.points_begin(), normalized.points_end(), length, Point(1, -1));
	correspondence[STROKE_N] = recognize_stroke_by_radical(
		normalized.points_begin(), normalized.points_end(), length, Point(1, 1));
	correspondence[STROKE_P] = recognize_stroke_by_radical(
		normalized.points_begin(), normalized.points_end(), length, Point(-1, 1));
	correspondence[STROKE_Y] = recognize_stroke_by_radical(
		normalized.points_begin(), normalized.points_end(), length, Point(-1, -1));

	correspondence[STROKE_SW] = recognize_stroke_by_radical(
		normalized.points_begin(), normalized.points_end(), length, Point(0, 1), Point(1, 0));
	correspondence[STROKE_HZG] = recognize_stroke_by_radical(
		normalized.points_begin(), normalized.points_end(), length, Point(1, 0), Point(0, 1));

	correspondence[STROKE_HZZ] = recognize_stroke_by_radical(
		normalized.points_begin(), normalized.points_end(), length,
		Point(1, 0), Point(0, 1), Point(1, 0));
	correspondence[STROKE_HPWG] = recognize_stroke_by_radical(
		normalized.points_begin(), normalized.points_end(), length,
		Point(1, 0), Point(-1, -1), Point(0, 1));
	correspondence[STROKE_SZWG] = recognize_stroke_by_radical(
		normalized.points_begin(), normalized.points_end(), length,
		Point(0, 1), Point(1, 0), Point(0, 1));

	correspondence[STROKE_HZZP] = recognize_stroke_by_radical(
		normalized.points_begin(), normalized.points_end(), length,
		Point(1, 0), Point(0, 1), Point(1, 0), Point(0, 1));

	std::copy(correspondence, correspondence + STROKE_TYPE_SIZE,
		std::ostream_iterator<double>(std::cerr, ",\t") );
	std::cerr << std::endl;

	return STROKE_H;
}

double
Recognizer::stroke_length(Point::iterator first, Point::iterator last) const
{
	double length = 0.0;
	for (Point::iterator pi = first;
	     pi != last - 1;
	     ++pi)
	{
		if (pi == last || (pi+1) == last)
			break;
		length += (*pi - *(pi+1)).abs();
	}
	return length;
}

double
Recognizer::recognize_stroke_by_radical(const Point & p, double length, const Point & comp) const
{
	return
		(comp.arg() - p.arg()) * (comp.arg() - p.arg()) *
		//< argument of the line, square to make it weights more when arguments same.
		(p.abs() / length);
		//< the length % of the whole stroke
}

double
Recognizer::recognize_stroke_by_radical(
	Point::iterator first, Point::iterator last, double length,
	const Point & comp1) const
{
	double possibility = 0.0;
	for (Point::iterator pi = first;
	     pi != last - 1;
	     ++pi)
	{
		Point::iterator p0 = pi, p1 = pi + 1;
		if (p0 == last || p1 == last)
			break;
		Point pp1 = *p1 - *p0;
		possibility += recognize_stroke_by_radical(pp1, length, comp1);
	}
	return _abs( possibility );
}

double
Recognizer::recognize_stroke_by_radical(
	Point::iterator first, Point::iterator last, double length,
	const Point & comp1, const Point & comp2) const
{
	// check point number...
	if ((first == last) || (first + 1 == last) || (first + 2 == last))
		return 9999.9;
	double possibility = 0.0;
	for (Point::iterator pi = first;
	     pi != last - 2;
	     ++pi)
	{
		Point::iterator p0 = pi, p1 = pi + 1, p2 = pi + 2;
		if (p0 == last || p1 == last || p2 == last)
			break;
		Point pp1 = *p1 - *p0, pp2 = *p2 - *p1;
		possibility += recognize_stroke_by_radical(pp1, length, comp1);
		possibility += recognize_stroke_by_radical(pp2, length, comp2);
	}

	return _abs( possibility );
}

double
Recognizer::recognize_stroke_by_radical(
	Point::iterator first, Point::iterator last, double length,
	const Point & comp1, const Point & comp2, const Point & comp3) const
{
	if ((first == last) || (first + 1 == last) || (first + 2 == last) || (first + 3 == last))
		return 9999.9;
	double possibility = 0.0;
	for (Point::iterator pi = first;
	     pi != last - 3;
	     ++pi)
	{
		Point::iterator p0 = pi, p1 = pi + 1, p2 = pi + 2, p3 = pi + 3;
		if (p0 == last || p1 == last || p2 == last || p3 == last)
			break;
		Point pp1 = *p1 - *p0, pp2 = *p2 - *p1, pp3 = *p3 - *p2;
		possibility += recognize_stroke_by_radical(pp1, length, comp1);
		possibility += recognize_stroke_by_radical(pp2, length, comp2);
		possibility += recognize_stroke_by_radical(pp3, length, comp3);
	}
	return _abs( possibility );
}

double
Recognizer::recognize_stroke_by_radical(
	Point::iterator first, Point::iterator last, double length,
	const Point & comp1, const Point & comp2, const Point & comp3, const Point & comp4) const
{
	if ((first == last) || (first + 1 == last) || (first + 2 == last) ||
	    (first + 3 == last) || (first + 4 == last))
		return 9999.9;
	double possibility = 0.0;
	for (Point::iterator pi = first;
	     pi != last - 4;
	     ++pi)
	{
		Point::iterator p0 = pi, p1 = pi + 1, p2 = pi + 2, p3 = pi + 3, p4 = pi + 4;
		if (p0 == last || p1 == last || p2 == last || p3 == last || p4 == last)
			break;
		Point pp1 = *p1 - *p0, pp2 = *p2 - *p1, pp3 = *p3 - *p2, pp4 = *p4 - *p3;
		possibility += recognize_stroke_by_radical(pp1, length, comp1);
		possibility += recognize_stroke_by_radical(pp2, length, comp2);
		possibility += recognize_stroke_by_radical(pp3, length, comp3);
		possibility += recognize_stroke_by_radical(pp4, length, comp4);
	}
	return _abs( possibility );
}
Recognizer::Recognizer() {};
Recognizer::Recognizer(const Recognizer &) {};
Recognizer & Recognizer::operator = (const Recognizer &) { return *this; };
Recognizer::~Recognizer() {};
