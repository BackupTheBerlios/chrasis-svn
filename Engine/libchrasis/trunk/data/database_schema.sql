CREATE TABLE c (
	c_id	INTEGER PRIMARY KEY	AUTOINCREMENT	NOT NULL,
	c_n	TEXT					NOT NULL,
	s_cnt	INTEGER					NOT NULL,
	smp_cnt	INTEGER  DEFAULT 1			NOT NULL,
	sp_hash	TEXT					NOT NULL
);
CREATE TABLE s (
	s_id	INTEGER PRIMARY KEY	AUTOINCREMENT	NOT NULL,
	c_id	INTEGER					NOT NULL,
	p_cnt	INTEGER					NOT NULL,
	seq	INTEGER					NOT NULL
);
CREATE TABLE p (
	p_id	INTEGER PRIMARY KEY	AUTOINCREMENT	NOT NULL,
	s_id	INTEGER					NOT NULL,
	seq	INTEGER					NOT NULL,
	x	INTEGER					NOT NULL,
	y	INTEGER					NOT NULL
);
CREATE VIEW c_s_p AS
	SELECT
		c.c_id	AS c_id,
		s.s_id	AS s_id,
		s.seq	AS s_seq,
		p.seq	AS p_seq,
		p.x	AS x,
		p.y	AS y
	FROM p, s, c WHERE
		p.s_id = s.s_id AND
		s.c_id = c.c_id
	ORDER BY
		s.c_id ASC, s.seq ASC, p.seq ASC;
CREATE VIEW stats AS
	SELECT
		SUM(smp_cnt)		AS total_sample_cnt,
		COUNT(DISTINCT c_n)	AS total_diff_chrs
	FROM c;
CREATE INDEX c_hash_scnt ON c (sp_hash,s_cnt);
CREATE INDEX c_n ON c (c_n);
CREATE INDEX s_cid_seq ON s (c_id,seq);
CREATE INDEX p_sid_seq ON p (s_id,seq);
CREATE TRIGGER c_id_change AFTER UPDATE OF c_id ON c
BEGIN
	UPDATE s SET c_id = new.c_id WHERE
		c_id = old.c_id;
END;
CREATE TRIGGER s_id_change AFTER UPDATE OF s_id ON s
BEGIN
	UPDATE p SET s_id = new.s_id WHERE
		s_id = old.s_id;
END;
