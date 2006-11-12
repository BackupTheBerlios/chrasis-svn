#ifndef _RECOGNIZER_H
#define _RECOGNIZER_H

#include "character.h"

class Recognizer
{
public:
	static Recognizer & Instance();

	// consult the Unicode UniHan Database (U4E00, U31C0)
	enum STROKE_TYPES
	{
		// 1 segment
		STROKE_H,	//< 4E00 一 CJK Character One (H)	橫

		STROKE_S,	//< 4E28 丨 CJK Character Line (S)	豎
				//< 31C1  CJK STROKE WG		彎勾

		STROKE_T,	//< 31C0  CJK STROKE T			挑

		STROKE_N,	//< 31CF  CJK STROKE N			捺
				//< 31C2  CJK STROKE XG		下勾
				//< 31C3  CJK STROKE BXG

		STROKE_P,	//< 	 /				撇（右上左下的捺）

		STROKE_Y,	//<	 \				揚（右下左上的挑）

		// 2 segments
		STROKE_SW,	//< 31C4  CJK STROKE SW		豎彎

		STROKE_HZG,	//< 31C6  CJK STROKE HZG		橫轉勾
				//< 31C7  CJK STROKE HP		橫撇

		// 3 segments
		STROKE_HZZ,	//< 31C5  CJK STROKE HZZ		橫轉轉
				//< 31CA  CJK STROKE HZT		橫轉挑
				//< 31C8  CJK STROKE HZWG		橫轉彎勾
				//< 31CD  CJK STROKE HZW		橫轉彎

		STROKE_HPWG,	//< 31CC  CJK STROKE HPWG		橫撇彎勾

		STROKE_SZWG,	//< 31C9  CJK STROKE SZWG		豎轉彎勾

		// 4 segments
		STROKE_HZZP,	//< 31CB  CJK STROKE HZZP		橫轉轉撇
				//< 31CE  CJK STROKE HZZZ		橫轉轉轉

		STROKE_TYPE_SIZE
	};

	Stroke normalize(Stroke &) const;
	Stroke scale(Stroke &) const;
	STROKE_TYPES recognize(Stroke &) const;

private:
	double stroke_length(Point::iterator, Point::iterator) const;

	double recognize_stroke_by_radical(const Point &, double, const Point &) const;
	double recognize_stroke_by_radical(Point::iterator, Point::iterator, double,
		const Point &) const;
	double recognize_stroke_by_radical(Point::iterator, Point::iterator, double,
		const Point &, const Point &) const;
	double recognize_stroke_by_radical(Point::iterator, Point::iterator, double,
		const Point &, const Point &, const Point &) const;
	double recognize_stroke_by_radical(Point::iterator, Point::iterator, double,
		const Point &, const Point &, const Point &, const Point &) const;

	Recognizer();
	Recognizer(const Recognizer &);
	Recognizer & operator=(const Recognizer &);
	~Recognizer();
};

#endif
