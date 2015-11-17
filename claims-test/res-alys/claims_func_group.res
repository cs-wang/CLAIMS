1. 
create table t1 (grp int, a int , c char(10) );
create projection on t1(grp, a, c) number = 1 partitioned on a;
insert into t1 values (1,1,"a");
insert into t1 values (2,2,"b");
insert into t1 values (2,3,"c");
insert into t1 values (3,4,"E");
insert into t1 values (3,5,"C");
insert into t1 values (3,6,"D");

select a,c,sum(a) from t1 group by a; Segmentation fault (core dumped)
select a,c,sum(a) from t1 where a > 10 group by a; Segmentation fault (core dumped)
select grp, sum(a),count(a),avg(a),std(a),variance(a),bit_or(a),bit_and(a),min(a),max(a),min(c),max(c) from t1 group by grp; Segmentation fault (core dumped)
select grp, sum(a)+count(a)+avg(a)+std(a)+variance(a)+bit_or(a)+bit_and(a)+min(a)+max(a)+min(c)+max(c) as sum from t1 group by grp; Segmentation fault (core dumped)


2. 
CREATE TABLE t1 (id int(11),value1 float(10,2));
create projection on t1(id, value1) number = 1 partitioned on id;
INSERT INTO t1 VALUES (1,0.00),(1,1.00), (1,2.00), (2,10.00), (2,11.00), (2,12.00); 
CREATE TABLE t2 (id int(11),name char(20)); 
create projection on t2(id, name) number = 1 partitioned on id;
INSERT INTO t2 VALUES (1,'Set One'),(2,'Set Two');

select id, avg(value1), std(value1), variance(value1) from t1 group by id; Segmentation fault (core dumped)
select name, avg(value1) from t1, t2 where t1.id = t2.id group by t1.id;  Segmentation fault (core dumped)

max min to date is not effective.

