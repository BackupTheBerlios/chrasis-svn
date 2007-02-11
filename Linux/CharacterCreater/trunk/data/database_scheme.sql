CREATE TABLE stroke_segments (
	seg_id		INTEGER		AUTOINCREMENT	NOT NULL,
	argument	REAL				NOT NULL,
	length		REAL				NOT NULL,
	sample_count	INTEGER		DEFAULT 1	NOT NULL,
	PRIMARY(seg_id, argument, length),
	UNIQUE(seg_id)
);

CREATE TABLE strokes (
	stroke_id	INTEGER		AUTOINCREMENT	NOT NULL,
	segments	INTEGER		DEFAULT 1	NOT NULL, /* segment count */
	seg1		INTEGER		DEFAULT 0	NOT NULL,
	seg2		INTEGER		DEFAULT 0	NOT NULL,
	seg3		INTEGER		DEFAULT 0	NOT NULL,
	seg4		INTEGER		DEFAULT 0	NOT NULL,
	seg5		INTEGER		DEFAULT 0	NOT NULL,
	seg6		INTEGER		DEFAULT 0	NOT NULL,
	seg7		INTEGER		DEFAULT 0	NOT NULL,
	seg8		INTEGER		DEFAULT 0	NOT NULL,
	seg9		INTEGER		DEFAULT 0	NOT NULL,
	PRIMARY(stroke_id)
	UNIQUE(seg_id)
);

SELECT stroke_id FROM strokes
WHERE
	segments = 2 AND
	seg1 = ( SELECT seg_id FROM stroke_segments WHERE
		argument BETWEEN 0.3 AND 0.5 ) AND	/* first segment, length ignored */
	seg2 = ( SELECT seg_id FROM stroke_segments WHERE
		argument BETWEEN 0.5 AND 0.8 AND
		length BETWEEN 6.5 AND 8.3 ) )
