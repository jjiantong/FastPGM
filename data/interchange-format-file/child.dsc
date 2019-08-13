belief network "unknown"
node BirthAsphyxia {
  type : discrete [ 2 ] = { "yes", "no" };
}
node HypDistrib {
  type : discrete [ 2 ] = { "Equal", "Unequal" };
}
node HypoxiaInO2 {
  type : discrete [ 3 ] = { "Mild", "Moderate", "Severe" };
}
node CO2 {
  type : discrete [ 3 ] = { "Normal", "Low", "High" };
}
node ChestXray {
  type : discrete [ 5 ] = { "Normal", "Oligaemic", "Plethoric", "Grd_Glass", "Asy/Patch" };
}
node Grunting {
  type : discrete [ 2 ] = { "yes", "no" };
}
node LVHreport {
  type : discrete [ 2 ] = { "yes", "no" };
}
node LowerBodyO2 {
  type : discrete [ 3 ] = { "<5", "5-12", "12+" };
}
node RUQO2 {
  type : discrete [ 3 ] = { "<5", "5-12", "12+" };
}
node CO2Report {
  type : discrete [ 2 ] = { "<7.5", ">=7.5" };
}
node XrayReport {
  type : discrete [ 5 ] = { "Normal", "Oligaemic", "Plethoric", "Grd_Glass", "Asy/Patchy" };
}
node Disease {
  type : discrete [ 6 ] = { "PFC", "TGA", "Fallot", "PAIVS", "TAPVD", "Lung" };
}
node GruntingReport {
  type : discrete [ 2 ] = { "yes", "no" };
}
node Age {
  type : discrete [ 3 ] = { "0-3_days", "4-10_days", "11-30_days" };
}
node LVH {
  type : discrete [ 2 ] = { "yes", "no" };
}
node DuctFlow {
  type : discrete [ 3 ] = { "Lt_to_Rt", "None", "Rt_to_Lt" };
}
node CardiacMixing {
  type : discrete [ 4 ] = { "None", "Mild", "Complete", "Transp." };
}
node LungParench {
  type : discrete [ 3 ] = { "Normal", "Congested", "Abnormal" };
}
node LungFlow {
  type : discrete [ 3 ] = { "Normal", "Low", "High" };
}
node Sick {
  type : discrete [ 2 ] = { "yes", "no" };
}
probability ( BirthAsphyxia ) {
   0.1, 0.9;
}
probability ( HypDistrib | DuctFlow, CardiacMixing ) {
  (0, 0) : 0.95, 0.05;
  (1, 0) : 0.95, 0.05;
  (2, 0) : 0.05, 0.95;
  (0, 1) : 0.95, 0.05;
  (1, 1) : 0.95, 0.05;
  (2, 1) : 0.5, 0.5;
  (0, 2) : 0.95, 0.05;
  (1, 2) : 0.95, 0.05;
  (2, 2) : 0.95, 0.05;
  (0, 3) : 0.95, 0.05;
  (1, 3) : 0.95, 0.05;
  (2, 3) : 0.5, 0.5;
}
probability ( HypoxiaInO2 | CardiacMixing, LungParench ) {
  (0, 0) : 0.93, 0.05, 0.02;
  (1, 0) : 0.1, 0.8, 0.1;
  (2, 0) : 0.1, 0.7, 0.2;
  (3, 0) : 0.02, 0.18, 0.80;
  (0, 1) : 0.15, 0.80, 0.05;
  (1, 1) : 0.10, 0.75, 0.15;
  (2, 1) : 0.05, 0.65, 0.30;
  (3, 1) : 0.1, 0.3, 0.6;
  (0, 2) : 0.7, 0.2, 0.1;
  (1, 2) : 0.10, 0.65, 0.25;
  (2, 2) : 0.1, 0.5, 0.4;
  (3, 2) : 0.02, 0.18, 0.80;
}
probability ( CO2 | LungParench ) {
  (0) : 0.8, 0.1, 0.1;
  (1) : 0.65, 0.05, 0.30;
  (2) : 0.45, 0.05, 0.50;
}
probability ( ChestXray | LungParench, LungFlow ) {
  (0, 0) : 0.90, 0.03, 0.03, 0.01, 0.03;
  (1, 0) : 0.05, 0.02, 0.15, 0.70, 0.08;
  (2, 0) : 0.05, 0.05, 0.05, 0.05, 0.80;
  (0, 1) : 0.14, 0.80, 0.02, 0.02, 0.02;
  (1, 1) : 0.05, 0.22, 0.08, 0.50, 0.15;
  (2, 1) : 0.05, 0.15, 0.05, 0.05, 0.70;
  (0, 2) : 0.15, 0.01, 0.79, 0.04, 0.01;
  (1, 2) : 0.05, 0.02, 0.40, 0.40, 0.13;
  (2, 2) : 0.24, 0.33, 0.03, 0.34, 0.06;
}
probability ( Grunting | LungParench, Sick ) {
  (0, 0) : 0.2, 0.8;
  (1, 0) : 0.4, 0.6;
  (2, 0) : 0.8, 0.2;
  (0, 1) : 0.05, 0.95;
  (1, 1) : 0.2, 0.8;
  (2, 1) : 0.6, 0.4;
}
probability ( LVHreport | LVH ) {
  (0) : 0.9, 0.1;
  (1) : 0.05, 0.95;
}
probability ( LowerBodyO2 | HypDistrib, HypoxiaInO2 ) {
  (0, 0) : 0.1, 0.3, 0.6;
  (1, 0) : 0.4, 0.5, 0.1;
  (0, 1) : 0.3, 0.6, 0.1;
  (1, 1) : 0.50, 0.45, 0.05;
  (0, 2) : 0.5, 0.4, 0.1;
  (1, 2) : 0.60, 0.35, 0.05;
}
probability ( RUQO2 | HypoxiaInO2 ) {
  (0) : 0.1, 0.3, 0.6;
  (1) : 0.3, 0.6, 0.1;
  (2) : 0.5, 0.4, 0.1;
}
probability ( CO2Report | CO2 ) {
  (0) : 0.9, 0.1;
  (1) : 0.9, 0.1;
  (2) : 0.1, 0.9;
}
probability ( XrayReport | ChestXray ) {
  (0) : 0.80, 0.06, 0.06, 0.02, 0.06;
  (1) : 0.10, 0.80, 0.02, 0.02, 0.06;
  (2) : 0.10, 0.02, 0.80, 0.02, 0.06;
  (3) : 0.08, 0.02, 0.10, 0.60, 0.20;
  (4) : 0.08, 0.02, 0.10, 0.10, 0.70;
}
probability ( Disease | BirthAsphyxia ) {
  (0) : 0.20, 0.30, 0.25, 0.15, 0.05, 0.05;
  (1) : 0.03061224, 0.33673469, 0.29591837, 0.23469388, 0.05102041, 0.05102041;
}
probability ( GruntingReport | Grunting ) {
  (0) : 0.8, 0.2;
  (1) : 0.1, 0.9;
}
probability ( Age | Disease, Sick ) {
  (0, 0) : 0.95, 0.03, 0.02;
  (1, 0) : 0.80, 0.15, 0.05;
  (2, 0) : 0.70, 0.15, 0.15;
  (3, 0) : 0.80, 0.15, 0.05;
  (4, 0) : 0.80, 0.15, 0.05;
  (5, 0) : 0.90, 0.08, 0.02;
  (0, 1) : 0.85, 0.10, 0.05;
  (1, 1) : 0.7, 0.2, 0.1;
  (2, 1) : 0.25, 0.25, 0.50;
  (3, 1) : 0.80, 0.15, 0.05;
  (4, 1) : 0.7, 0.2, 0.1;
  (5, 1) : 0.80, 0.15, 0.05;
}
probability ( LVH | Disease ) {
  (0) : 0.1, 0.9;
  (1) : 0.1, 0.9;
  (2) : 0.1, 0.9;
  (3) : 0.9, 0.1;
  (4) : 0.05, 0.95;
  (5) : 0.1, 0.9;
}
probability ( DuctFlow | Disease ) {
  (0) : 0.15, 0.05, 0.80;
  (1) : 0.1, 0.8, 0.1;
  (2) : 0.8, 0.2, 0.0;
  (3) : 1.0, 0.0, 0.0;
  (4) : 0.33, 0.33, 0.34;
  (5) : 0.2, 0.4, 0.4;
}
probability ( CardiacMixing | Disease ) {
  (0) : 0.40, 0.43, 0.15, 0.02;
  (1) : 0.02, 0.09, 0.09, 0.80;
  (2) : 0.02, 0.16, 0.80, 0.02;
  (3) : 0.01, 0.02, 0.95, 0.02;
  (4) : 0.01, 0.03, 0.95, 0.01;
  (5) : 0.40, 0.53, 0.05, 0.02;
}
probability ( LungParench | Disease ) {
  (0) : 0.6, 0.1, 0.3;
  (1) : 0.80, 0.05, 0.15;
  (2) : 0.80, 0.05, 0.15;
  (3) : 0.80, 0.05, 0.15;
  (4) : 0.1, 0.6, 0.3;
  (5) : 0.03, 0.25, 0.72;
}
probability ( LungFlow | Disease ) {
  (0) : 0.30, 0.65, 0.05;
  (1) : 0.20, 0.05, 0.75;
  (2) : 0.15, 0.80, 0.05;
  (3) : 0.10, 0.85, 0.05;
  (4) : 0.3, 0.1, 0.6;
  (5) : 0.7, 0.1, 0.2;
}
probability ( Sick | Disease ) {
  (0) : 0.4, 0.6;
  (1) : 0.3, 0.7;
  (2) : 0.2, 0.8;
  (3) : 0.3, 0.7;
  (4) : 0.7, 0.3;
  (5) : 0.7, 0.3;
}
