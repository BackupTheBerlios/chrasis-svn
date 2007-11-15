CREATE TABLE characters (
	c_id	INTEGER PRIMARY KEY	AUTOINCREMENT	NOT NULL,
	c_name	TEXT					NOT NULL,
	c_hash	TEXT					NOT NULL,
	s_cnt	INTEGER					NOT NULL,
	smp_cnt	INTEGER	DEFAULT 1			NOT NULL
);
CREATE TABLE strokes (
	s_id	INTEGER PRIMARY KEY	AUTOINCREMENT	NOT NULL,
	c_id	INTEGER					NOT NULL,
	seq	INTEGER					NOT NULL,
	ss_id	INTEGER					NOT NULL,
	lt_x	INTEGER					NOT NULL,
	lt_y	INTEGER					NOT NULL,
	rb_x	INTEGER					NOT NULL,
	rb_y	INTEGER					NOT NULL
);
CREATE TABLE stroke_shapes (
	ss_id	INTEGER	PRIMARY KEY	AUTOINCREMENT	NOT NULL,
	pt_cnt	INTEGER					NOT NULL,
	smp_cnt	INTEGER	DEFAULT 1			NOT NULL
);
CREATE TABLE points (
	p_id	INTEGER PRIMARY KEY	AUTOINCREMENT	NOT NULL,
	ss_id	INTEGER					NOT NULL,
	seq	INTEGER					NOT NULL,
	x	INTEGER					NOT NULL,
	y	INTEGER					NOT NULL
);
CREATE INDEX sshape_ptcnt_id ON stroke_shapes (
	pt_cnt,
	ss_id
);
CREATE INDEX pts_ssid_seq_xy ON points (
	ss_id,
	seq,
	x,
	y
);
CREATE INDEX chr_scnt_hash_name ON characters (
	s_cnt,
	c_hash,
	c_name
);
CREATE INDEX str_id_ssid_ltrbxy ON strokes (
	ss_id,
	lt_x, lt_y,
	rb_x, rb_y
);
