belief network "unknown"
node asia {
  type : discrete [ 2 ] = { "yes", "no" };
}
node tub {
  type : discrete [ 2 ] = { "yes", "no" };
}
node smoke {
  type : discrete [ 2 ] = { "yes", "no" };
}
node lung {
  type : discrete [ 2 ] = { "yes", "no" };
}
node bronc {
  type : discrete [ 2 ] = { "yes", "no" };
}
node either {
  type : discrete [ 2 ] = { "yes", "no" };
}
node xray {
  type : discrete [ 2 ] = { "yes", "no" };
}
node dysp {
  type : discrete [ 2 ] = { "yes", "no" };
}
probability ( asia ) {
   0.01, 0.99;
}
probability ( tub | asia ) {
  (0) : 0.05, 0.95;
  (1) : 0.01, 0.99;
}
probability ( smoke ) {
   0.5, 0.5;
}
probability ( lung | smoke ) {
  (0) : 0.1, 0.9;
  (1) : 0.01, 0.99;
}
probability ( bronc | smoke ) {
  (0) : 0.6, 0.4;
  (1) : 0.3, 0.7;
}
probability ( either | lung, tub ) {
  (0, 0) : 1.0, 0.0;
  (1, 0) : 1.0, 0.0;
  (0, 1) : 1.0, 0.0;
  (1, 1) : 0.0, 1.0;
}
probability ( xray | either ) {
  (0) : 0.98, 0.02;
  (1) : 0.05, 0.95;
}
probability ( dysp | bronc, either ) {
  (0, 0) : 0.9, 0.1;
  (1, 0) : 0.7, 0.3;
  (0, 1) : 0.8, 0.2;
  (1, 1) : 0.1, 0.9;
}
