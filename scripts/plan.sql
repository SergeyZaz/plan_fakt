--
-- PostgreSQL database dump
--

-- Dumped from database version 9.4.21
-- Dumped by pg_dump version 9.4.21
-- Started on 2019-07-02 20:59:23

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- TOC entry 2 (class 3079 OID 11855)
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- TOC entry 2095 (class 0 OID 0)
-- Dependencies: 2
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


--
-- TOC entry 1 (class 3079 OID 18322)
-- Name: adminpack; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS adminpack WITH SCHEMA pg_catalog;


--
-- TOC entry 2096 (class 0 OID 0)
-- Dependencies: 1
-- Name: EXTENSION adminpack; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION adminpack IS 'administrative functions for PostgreSQL';


SET default_tablespace = '';

SET default_with_oids = false;

--
-- TOC entry 174 (class 1259 OID 18331)
-- Name: accounts; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE public.accounts (
    id integer NOT NULL,
    name text,
    comment text,
    type integer,
    ur_person integer,
    bik text,
    bank text,
    account text,
    val double precision DEFAULT 0
);


ALTER TABLE public.accounts OWNER TO postgres;

--
-- TOC entry 2097 (class 0 OID 0)
-- Dependencies: 174
-- Name: TABLE accounts; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE public.accounts IS 'Справочник счетов';


--
-- TOC entry 2098 (class 0 OID 0)
-- Dependencies: 174
-- Name: COLUMN accounts.name; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.accounts.name IS 'Краткое название';


--
-- TOC entry 2099 (class 0 OID 0)
-- Dependencies: 174
-- Name: COLUMN accounts.comment; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.accounts.comment IS 'Комментарий';


--
-- TOC entry 2100 (class 0 OID 0)
-- Dependencies: 174
-- Name: COLUMN accounts.type; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.accounts.type IS 'Тип: 0-наличный/1-безналичный';


--
-- TOC entry 2101 (class 0 OID 0)
-- Dependencies: 174
-- Name: COLUMN accounts.ur_person; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.accounts.ur_person IS 'Юр.лицо';


--
-- TOC entry 2102 (class 0 OID 0)
-- Dependencies: 174
-- Name: COLUMN accounts.bik; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.accounts.bik IS 'БИК';


--
-- TOC entry 2103 (class 0 OID 0)
-- Dependencies: 174
-- Name: COLUMN accounts.bank; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.accounts.bank IS 'Банк';


--
-- TOC entry 2104 (class 0 OID 0)
-- Dependencies: 174
-- Name: COLUMN accounts.account; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.accounts.account IS 'Номер счета';


--
-- TOC entry 2105 (class 0 OID 0)
-- Dependencies: 174
-- Name: COLUMN accounts.val; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.accounts.val IS 'Текущий остаток';


--
-- TOC entry 175 (class 1259 OID 18338)
-- Name: accounts_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.accounts_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.accounts_id_seq OWNER TO postgres;

--
-- TOC entry 2106 (class 0 OID 0)
-- Dependencies: 175
-- Name: accounts_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.accounts_id_seq OWNED BY public.accounts.id;


--
-- TOC entry 176 (class 1259 OID 18340)
-- Name: operation2project; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE public.operation2project (
    key bigint NOT NULL,
    val bigint NOT NULL
);


ALTER TABLE public.operation2project OWNER TO postgres;

--
-- TOC entry 2107 (class 0 OID 0)
-- Dependencies: 176
-- Name: TABLE operation2project; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE public.operation2project IS 'Связь операции с проектами';


--
-- TOC entry 2108 (class 0 OID 0)
-- Dependencies: 176
-- Name: COLUMN operation2project.key; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.operation2project.key IS 'Операция';


--
-- TOC entry 2109 (class 0 OID 0)
-- Dependencies: 176
-- Name: COLUMN operation2project.val; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.operation2project.val IS 'Проект';


--
-- TOC entry 177 (class 1259 OID 18343)
-- Name: operations; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE public.operations (
    id integer NOT NULL,
    date date,
    type integer,
    comment text,
    ur_person integer,
    partner integer,
    section integer DEFAULT 0,
    project integer DEFAULT 0,
    val double precision,
    account integer,
    num integer DEFAULT 0
);


ALTER TABLE public.operations OWNER TO postgres;

--
-- TOC entry 2110 (class 0 OID 0)
-- Dependencies: 177
-- Name: TABLE operations; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE public.operations IS 'Операции';


--
-- TOC entry 2111 (class 0 OID 0)
-- Dependencies: 177
-- Name: COLUMN operations.date; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.operations.date IS 'Дата операции';


--
-- TOC entry 2112 (class 0 OID 0)
-- Dependencies: 177
-- Name: COLUMN operations.type; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.operations.type IS 'Тип: 0-Поступление/1-Выплата/2-Перемещение';


--
-- TOC entry 2113 (class 0 OID 0)
-- Dependencies: 177
-- Name: COLUMN operations.comment; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.operations.comment IS 'Комментарий';


--
-- TOC entry 2114 (class 0 OID 0)
-- Dependencies: 177
-- Name: COLUMN operations.ur_person; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.operations.ur_person IS 'Юр.лицо';


--
-- TOC entry 2115 (class 0 OID 0)
-- Dependencies: 177
-- Name: COLUMN operations.partner; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.operations.partner IS 'контрагент';


--
-- TOC entry 2116 (class 0 OID 0)
-- Dependencies: 177
-- Name: COLUMN operations.section; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.operations.section IS 'статья';


--
-- TOC entry 2117 (class 0 OID 0)
-- Dependencies: 177
-- Name: COLUMN operations.project; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.operations.project IS 'проект';


--
-- TOC entry 2118 (class 0 OID 0)
-- Dependencies: 177
-- Name: COLUMN operations.val; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.operations.val IS 'Сумма';


--
-- TOC entry 2119 (class 0 OID 0)
-- Dependencies: 177
-- Name: COLUMN operations.account; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.operations.account IS 'Счет';


--
-- TOC entry 2120 (class 0 OID 0)
-- Dependencies: 177
-- Name: COLUMN operations.num; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.operations.num IS 'Номер операции';


--
-- TOC entry 178 (class 1259 OID 18352)
-- Name: operations_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.operations_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.operations_id_seq OWNER TO postgres;

--
-- TOC entry 2121 (class 0 OID 0)
-- Dependencies: 178
-- Name: operations_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.operations_id_seq OWNED BY public.operations.id;


--
-- TOC entry 179 (class 1259 OID 18354)
-- Name: partners; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE public.partners (
    id integer NOT NULL,
    name text,
    full_name text,
    comment text,
    inn text,
    kpp text,
    section_plus integer DEFAULT 0,
    section_minus integer DEFAULT (-1)
);


ALTER TABLE public.partners OWNER TO postgres;

--
-- TOC entry 2122 (class 0 OID 0)
-- Dependencies: 179
-- Name: TABLE partners; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE public.partners IS 'Справочник контрагентов';


--
-- TOC entry 2123 (class 0 OID 0)
-- Dependencies: 179
-- Name: COLUMN partners.name; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.partners.name IS 'Краткое название';


--
-- TOC entry 2124 (class 0 OID 0)
-- Dependencies: 179
-- Name: COLUMN partners.full_name; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.partners.full_name IS 'Полное название';


--
-- TOC entry 2125 (class 0 OID 0)
-- Dependencies: 179
-- Name: COLUMN partners.comment; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.partners.comment IS 'Комментарий';


--
-- TOC entry 2126 (class 0 OID 0)
-- Dependencies: 179
-- Name: COLUMN partners.inn; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.partners.inn IS 'ИНН';


--
-- TOC entry 2127 (class 0 OID 0)
-- Dependencies: 179
-- Name: COLUMN partners.kpp; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.partners.kpp IS 'КПП';


--
-- TOC entry 2128 (class 0 OID 0)
-- Dependencies: 179
-- Name: COLUMN partners.section_plus; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.partners.section_plus IS 'доходная статья по умолчанию';


--
-- TOC entry 2129 (class 0 OID 0)
-- Dependencies: 179
-- Name: COLUMN partners.section_minus; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.partners.section_minus IS 'расходная статья по умолчанию';


--
-- TOC entry 180 (class 1259 OID 18362)
-- Name: partners_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.partners_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.partners_id_seq OWNER TO postgres;

--
-- TOC entry 2130 (class 0 OID 0)
-- Dependencies: 180
-- Name: partners_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.partners_id_seq OWNED BY public.partners.id;


--
-- TOC entry 181 (class 1259 OID 18364)
-- Name: projects; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE public.projects (
    id integer NOT NULL,
    name text,
    comment text
);


ALTER TABLE public.projects OWNER TO postgres;

--
-- TOC entry 2131 (class 0 OID 0)
-- Dependencies: 181
-- Name: TABLE projects; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE public.projects IS 'Проекты';


--
-- TOC entry 2132 (class 0 OID 0)
-- Dependencies: 181
-- Name: COLUMN projects.name; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.projects.name IS 'Название';


--
-- TOC entry 2133 (class 0 OID 0)
-- Dependencies: 181
-- Name: COLUMN projects.comment; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.projects.comment IS 'Комментарий';


--
-- TOC entry 182 (class 1259 OID 18370)
-- Name: projects_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.projects_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.projects_id_seq OWNER TO postgres;

--
-- TOC entry 2134 (class 0 OID 0)
-- Dependencies: 182
-- Name: projects_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.projects_id_seq OWNED BY public.projects.id;


--
-- TOC entry 183 (class 1259 OID 18372)
-- Name: sections; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE public.sections (
    id integer NOT NULL,
    name text,
    type integer,
    parent integer DEFAULT 0
);


ALTER TABLE public.sections OWNER TO postgres;

--
-- TOC entry 2135 (class 0 OID 0)
-- Dependencies: 183
-- Name: TABLE sections; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE public.sections IS 'Учетные статьи';


--
-- TOC entry 2136 (class 0 OID 0)
-- Dependencies: 183
-- Name: COLUMN sections.name; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.sections.name IS 'Название';


--
-- TOC entry 2137 (class 0 OID 0)
-- Dependencies: 183
-- Name: COLUMN sections.type; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.sections.type IS 'Тип: 0-Доходы/1-Расходы/2-Активы/3-Обязательства/4-Капитал';


--
-- TOC entry 2138 (class 0 OID 0)
-- Dependencies: 183
-- Name: COLUMN sections.parent; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.sections.parent IS 'Относится к:';


--
-- TOC entry 184 (class 1259 OID 18379)
-- Name: sections_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.sections_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.sections_id_seq OWNER TO postgres;

--
-- TOC entry 2139 (class 0 OID 0)
-- Dependencies: 184
-- Name: sections_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.sections_id_seq OWNED BY public.sections.id;


--
-- TOC entry 185 (class 1259 OID 18381)
-- Name: ur_persons; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE public.ur_persons (
    id integer NOT NULL,
    name text,
    full_name text,
    comment text,
    inn text,
    kpp text
);


ALTER TABLE public.ur_persons OWNER TO postgres;

--
-- TOC entry 2140 (class 0 OID 0)
-- Dependencies: 185
-- Name: TABLE ur_persons; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE public.ur_persons IS 'Справочник юрлиц';


--
-- TOC entry 2141 (class 0 OID 0)
-- Dependencies: 185
-- Name: COLUMN ur_persons.name; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.ur_persons.name IS 'Краткое название';


--
-- TOC entry 2142 (class 0 OID 0)
-- Dependencies: 185
-- Name: COLUMN ur_persons.full_name; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.ur_persons.full_name IS 'Полное название';


--
-- TOC entry 2143 (class 0 OID 0)
-- Dependencies: 185
-- Name: COLUMN ur_persons.comment; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.ur_persons.comment IS 'Комментарий';


--
-- TOC entry 2144 (class 0 OID 0)
-- Dependencies: 185
-- Name: COLUMN ur_persons.inn; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.ur_persons.inn IS 'ИНН';


--
-- TOC entry 2145 (class 0 OID 0)
-- Dependencies: 185
-- Name: COLUMN ur_persons.kpp; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN public.ur_persons.kpp IS 'КПП';


--
-- TOC entry 186 (class 1259 OID 18387)
-- Name: ur_persons_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.ur_persons_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.ur_persons_id_seq OWNER TO postgres;

--
-- TOC entry 2146 (class 0 OID 0)
-- Dependencies: 186
-- Name: ur_persons_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.ur_persons_id_seq OWNED BY public.ur_persons.id;


--
-- TOC entry 1923 (class 2604 OID 18389)
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.accounts ALTER COLUMN id SET DEFAULT nextval('public.accounts_id_seq'::regclass);


--
-- TOC entry 1927 (class 2604 OID 18390)
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.operations ALTER COLUMN id SET DEFAULT nextval('public.operations_id_seq'::regclass);


--
-- TOC entry 1930 (class 2604 OID 18391)
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.partners ALTER COLUMN id SET DEFAULT nextval('public.partners_id_seq'::regclass);


--
-- TOC entry 1931 (class 2604 OID 18392)
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.projects ALTER COLUMN id SET DEFAULT nextval('public.projects_id_seq'::regclass);


--
-- TOC entry 1933 (class 2604 OID 18393)
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sections ALTER COLUMN id SET DEFAULT nextval('public.sections_id_seq'::regclass);


--
-- TOC entry 1934 (class 2604 OID 18394)
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ur_persons ALTER COLUMN id SET DEFAULT nextval('public.ur_persons_id_seq'::regclass);


--
-- TOC entry 2074 (class 0 OID 18331)
-- Dependencies: 174
-- Data for Name: accounts; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- TOC entry 2147 (class 0 OID 0)
-- Dependencies: 175
-- Name: accounts_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.accounts_id_seq', 1, true);


--
-- TOC entry 2076 (class 0 OID 18340)
-- Dependencies: 176
-- Data for Name: operation2project; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- TOC entry 2077 (class 0 OID 18343)
-- Dependencies: 177
-- Data for Name: operations; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- TOC entry 2148 (class 0 OID 0)
-- Dependencies: 178
-- Name: operations_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.operations_id_seq', 1, true);


--
-- TOC entry 2079 (class 0 OID 18354)
-- Dependencies: 179
-- Data for Name: partners; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- TOC entry 2149 (class 0 OID 0)
-- Dependencies: 180
-- Name: partners_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.partners_id_seq', 1, true);


--
-- TOC entry 2081 (class 0 OID 18364)
-- Dependencies: 181
-- Data for Name: projects; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.projects VALUES (0, ' Нераспределенные', 'проект по умолчанию (удалить нельзя)');


--
-- TOC entry 2150 (class 0 OID 0)
-- Dependencies: 182
-- Name: projects_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.projects_id_seq', 1, true);


--
-- TOC entry 2083 (class 0 OID 18372)
-- Dependencies: 183
-- Data for Name: sections; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.sections VALUES (0, ' Нераспределенный доход', 0, 0);
INSERT INTO public.sections VALUES (-1, ' Нераспределенный расход', 1, 0);


--
-- TOC entry 2151 (class 0 OID 0)
-- Dependencies: 184
-- Name: sections_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.sections_id_seq', 2, true);


--
-- TOC entry 2085 (class 0 OID 18381)
-- Dependencies: 185
-- Data for Name: ur_persons; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- TOC entry 2152 (class 0 OID 0)
-- Dependencies: 186
-- Name: ur_persons_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.ur_persons_id_seq', 1, true);


--
-- TOC entry 1936 (class 2606 OID 18396)
-- Name: accounts_account_key; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY public.accounts
    ADD CONSTRAINT accounts_account_key UNIQUE (account);


--
-- TOC entry 1938 (class 2606 OID 18398)
-- Name: accounts_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY public.accounts
    ADD CONSTRAINT accounts_pkey PRIMARY KEY (id);


--
-- TOC entry 1950 (class 2606 OID 18400)
-- Name: articles_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY public.sections
    ADD CONSTRAINT articles_pkey PRIMARY KEY (id);


--
-- TOC entry 1940 (class 2606 OID 18402)
-- Name: operation2project_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY public.operation2project
    ADD CONSTRAINT operation2project_pkey PRIMARY KEY (key, val);


--
-- TOC entry 1942 (class 2606 OID 18404)
-- Name: operations_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY public.operations
    ADD CONSTRAINT operations_pkey PRIMARY KEY (id);


--
-- TOC entry 1944 (class 2606 OID 18406)
-- Name: partners_inn_key; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY public.partners
    ADD CONSTRAINT partners_inn_key UNIQUE (inn);


--
-- TOC entry 1946 (class 2606 OID 18408)
-- Name: partners_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY public.partners
    ADD CONSTRAINT partners_pkey PRIMARY KEY (id);


--
-- TOC entry 1948 (class 2606 OID 18410)
-- Name: projects_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY public.projects
    ADD CONSTRAINT projects_pkey PRIMARY KEY (id);


--
-- TOC entry 1952 (class 2606 OID 18412)
-- Name: ur_persons_inn_key; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY public.ur_persons
    ADD CONSTRAINT ur_persons_inn_key UNIQUE (inn);


--
-- TOC entry 1954 (class 2606 OID 18414)
-- Name: ur_persons_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY public.ur_persons
    ADD CONSTRAINT ur_persons_pkey PRIMARY KEY (id);


--
-- TOC entry 1955 (class 2606 OID 18415)
-- Name: fk_accounts_ref_ur_persons; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.accounts
    ADD CONSTRAINT fk_accounts_ref_ur_persons FOREIGN KEY (ur_person) REFERENCES public.ur_persons(id) ON UPDATE CASCADE ON DELETE SET DEFAULT;


--
-- TOC entry 1956 (class 2606 OID 18420)
-- Name: fk_operation2project_ref_operation; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.operation2project
    ADD CONSTRAINT fk_operation2project_ref_operation FOREIGN KEY (key) REFERENCES public.operations(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1957 (class 2606 OID 18425)
-- Name: fk_operation2project_ref_project; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.operation2project
    ADD CONSTRAINT fk_operation2project_ref_project FOREIGN KEY (val) REFERENCES public.projects(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1958 (class 2606 OID 18430)
-- Name: fk_operations_ref_accounts; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.operations
    ADD CONSTRAINT fk_operations_ref_accounts FOREIGN KEY (account) REFERENCES public.accounts(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1959 (class 2606 OID 18435)
-- Name: fk_operations_ref_partners; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.operations
    ADD CONSTRAINT fk_operations_ref_partners FOREIGN KEY (partner) REFERENCES public.partners(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1960 (class 2606 OID 18440)
-- Name: fk_operations_ref_projects; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.operations
    ADD CONSTRAINT fk_operations_ref_projects FOREIGN KEY (project) REFERENCES public.projects(id) ON DELETE SET DEFAULT;


--
-- TOC entry 1961 (class 2606 OID 18445)
-- Name: fk_operations_ref_sections; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.operations
    ADD CONSTRAINT fk_operations_ref_sections FOREIGN KEY (section) REFERENCES public.sections(id) ON DELETE SET DEFAULT;


--
-- TOC entry 1962 (class 2606 OID 18450)
-- Name: fk_operations_ref_ur_persons; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.operations
    ADD CONSTRAINT fk_operations_ref_ur_persons FOREIGN KEY (ur_person) REFERENCES public.ur_persons(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1963 (class 2606 OID 18455)
-- Name: fk_partners_ref_sections_minus; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.partners
    ADD CONSTRAINT fk_partners_ref_sections_minus FOREIGN KEY (section_minus) REFERENCES public.sections(id) ON DELETE SET DEFAULT;


--
-- TOC entry 1964 (class 2606 OID 18460)
-- Name: fk_partners_ref_sections_plus; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.partners
    ADD CONSTRAINT fk_partners_ref_sections_plus FOREIGN KEY (section_plus) REFERENCES public.sections(id) ON DELETE SET DEFAULT;


--
-- TOC entry 2094 (class 0 OID 0)
-- Dependencies: 8
-- Name: SCHEMA public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


-- Completed on 2019-07-02 20:59:24

--
-- PostgreSQL database dump complete
--

