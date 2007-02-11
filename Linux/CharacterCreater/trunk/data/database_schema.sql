/* tables */

CREATE TABLE IF NOT EXISTS points (
	pt_id		INTEGER	PRIMARY KEY	AUTOINCREMENT	NOT NULL,
	stroke_id	INTEGER					NOT NULL,
	sequence	INTEGER					NOT NULL,
	x		REAL					NOT NULL,
	y		REAL					NOT NULL
);

CREATE TABLE IF NOT EXISTS strokes (
	stroke_id	INTEGER	PRIMARY KEY	AUTOINCREMENT	NOT NULL,
	character_id	INTEGER					NOT NULL,
	pt_count	INTEGER					NOT NULL,
	sequence	INTEGER					NOT NULL
);

CREATE TABLE IF NOT EXISTS characters (
	character_id	INTEGER PRIMARY KEY	AUTOINCREMENT	NOT NULL,
	character_name	TEXT					NOT NULL,
	stroke_count	INTEGER					NOT NULL,
	sample_count	INTEGER DEFAULT 1			NOT NULL
);

/* triggers */

CREATE TRIGGER IF NOT EXISTS 
	strokes_id_change 
AFTER UPDATE OF
	stroke_id ON strokes 
BEGIN
	UPDATE
		points
	SET
		stroke_id = new.stroke_id
	WHERE
		stroke_id = old.stroke_id;
END;

CREATE TRIGGER IF NOT EXISTS
	characters_id_change
AFTER UPDATE OF
	character_id ON characters
BEGIN
	UPDATE
		strokes
	SET
		character_id = new.character_id
	WHERE
		character_id = old.character_id;
END;

/* views */

CREATE VIEW IF NOT EXISTS chr_pts AS
	SELECT
		characters.character_id	AS character_id,
		strokes.stroke_id	AS stroke_id,
		strokes.sequence	AS stroke_seq,
		points.sequence		AS point_seq,
		points.x 		AS x,
		points.y 		AS y
	FROM points, strokes, characters WHERE
		points.stroke_id = strokes.stroke_id AND
		strokes.character_id = characters.character_id
	ORDER BY
		strokes.character_id ASC,
		strokes.sequence ASC,
		points.sequence ASC;
