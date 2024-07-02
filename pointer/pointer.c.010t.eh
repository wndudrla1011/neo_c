
;; Function main (main, funcdef_no=13, decl_uid=3220, cgraph_uid=13, symbol_order=13)

main ()
{
  int * cursor;
  int val3;
  int val2;
  int val1;
  int D.3227;

  val1 = 10;
  val2 = 20;
  val3 = 30;
  printf ("\xeb\xa9\x94\xeb\xaa\xa8\xeb\xa6\xac\xec\x83\x81 val1 \xec\x9c\x84\xec\xb9\x98 : %d\n", &val1);
  printf ("\xeb\xa9\x94\xeb\xaa\xa8\xeb\xa6\xac\xec\x83\x81 val2 \xec\x9c\x84\xec\xb9\x98 : %d\n", &val2);
  printf ("\xeb\xa9\x94\xeb\xaa\xa8\xeb\xa6\xac\xec\x83\x81 val3 \xec\x9c\x84\xec\xb9\x98 : %d\n\n", &val3);
  cursor = &val1;
  val1.0_1 = val1;
  printf ("val1\xec\x9d\x98 \xec\xa3\xbc\xec\x86\x8c: %d, \xea\xb0\x92: %d\n", &val1, val1.0_1);
  _2 = *cursor;
  printf ("cursor\xec\x9d\x98 \xec\xa3\xbc\xec\x86\x8c: %d, \xea\xb0\x92: %d\n\n", cursor, _2);
  _3 = *cursor;
  _4 = _3 + 5;
  *cursor = _4;
  val1.1_5 = val1;
  printf ("val1\xec\x9d\x98 \xec\xa3\xbc\xec\x86\x8c: %d, \xea\xb0\x92: %d\n", &val1, val1.1_5);
  _6 = *cursor;
  printf ("cursor\xec\x9d\x98 \xec\xa3\xbc\xec\x86\x8c: %d, \xea\xb0\x92: %d\n", cursor, _6);
  D.3227 = 0;
  goto <D.3229>;
  <D.3229>:
  val1 = {CLOBBER};
  val2 = {CLOBBER};
  val3 = {CLOBBER};
  goto <D.3228>;
  D.3227 = 0;
  goto <D.3228>;
  <D.3228>:
  return D.3227;
}


