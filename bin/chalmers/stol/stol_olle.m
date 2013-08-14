%  ----------------------------------------
%	HALVA VIKTEN DUBBLA STYRKAN
%	----------------------------------------
%
%	Enkel CALFEM-modell av Olles stol i 3D.
%
%	----------------------------------------
%	Last modified 2000-05-31 Pierre Olsson
%	----------------------------------------

% Tömmer minnet
clear all;

% Skapar en matris med frihetsgradsnummer per nod
j=0;
for i=1:23
   Dof(i,:)=[j+1 j+2 j+3 j+4 j+5 j+6];
   j=j+6;
end;

% Skapar en "stöd-Edof" för noderna för att hålla reda på
% mellan vilka noder som balkarna går.
helpEdof=[1  1 5;
   		2  2 9;
   		3  3 10;
   		4  4 14;
   		5  5 6;
   		6  6 7;
   		7  7 8;
   		8  8 9;
   		9  8 11;
   		10 6 13;
   		11 14 13;
   		12 13 12;
   		13 12 11;
   		14 11 10;
   		15 5 16;
   		16 9 17;
   		17 14 20;
   		18 20 15;
   		19 15 16;
   		20 10 19;
   		21 19 18;
   		22 18 17;
   		23 17 23;
   		24 23 22;
   		25 22 21;
			26 21 16];
      
% Skapar en riktig Edof med frihetsgradsnummer.
for i=1:size(helpEdof,1)
   Edof(i,:)=[i Dof(helpEdof(i,2),1) Dof(helpEdof(i,2),2), ...
         		 Dof(helpEdof(i,2),3) Dof(helpEdof(i,2),4), ...
                Dof(helpEdof(i,2),5) Dof(helpEdof(i,2),6), ...
                Dof(helpEdof(i,3),1) Dof(helpEdof(i,3),2), ...
         		 Dof(helpEdof(i,3),3) Dof(helpEdof(i,3),4), ...
                Dof(helpEdof(i,3),5) Dof(helpEdof(i,3),6)];
end;

% Matris med aktuella punkter i stolen.
Coord=[0.0    0.0    0.0;
   	 0.430  0.0    0.0;
   	 0.465  0.380  0.0;
  		-0.035  0.380  0.0;
   	 0.0    0.0    0.390;
   	 0.050  0.0    0.390;
   	 0.215  0.0    0.390;
   	 0.380  0.0    0.390;
   	 0.430  0.0    0.390;
       0.465  0.390  0.390;
       0.380  0.390  0.390;
       0.215  0.390  0.390;
       0.050  0.390  0.390;
      -0.035  0.390  0.390;
      -0.035 -0.050  0.620;
       0.0   -0.050  0.620;
       0.430 -0.050  0.620;
       0.465 -0.050  0.620;
       0.465  0.340  0.620;
      -0.035  0.340  0.620;
       0.0   -0.100  0.810;
       0.215 -0.110  0.860;
       0.430 -0.100  0.810];
    
% Delar upp Coord i ex,ey,ez. 
[ex,ey,ez]=coordxtr(Edof,Coord,Dof,2);


% Bestämmer orienteringen för lokal z-axel, [xz yz zz].
for i=1:26
   eo(i,:)=[1 1 1];
end;

% Randvillkor.
bc=[1 0; 2 0; 3 0;
    7 0; 8 0; 9 0;
    12 0; 14 0; 
    19 0; 20 0;];
 
% Allokerar styvhetsmatris och kraftvektor.
K=zeros(138);
f=zeros(138,1);

% Material parametrar för balkarna (här samma för alla).
E=10e9; G=0.7e9; A=0.03^2; 
Iy=0.03^4/12; Iz=Iy; Kv=0.14*0.03^4;
for i=1:26
   ep(i,:)=[E G A Iy Iz Kv];
end;

% Skapar lokala styvhetsmatriser och assemblerar dem.
for i=1:26
   Ke=beam3e(ex(i,:),ey(i,:),ez(i,:),eo(i,:),ep(i,:));
   K=assem(Edof(i,:),K,Ke);
end;

% Sätter de yttre krafterna.
f(56)=-600;
f(33)=-100;
f(75)=-100;
f(69)=-100;
f(63)=-100;
f(45)=-100;
f(39)=-100;

% Löser ekvationssystemet.
[a,Q]=solveq(K,f,bc);

% Plockar ut de lokala förskjutningarna.
ed=extract(Edof,a);

% Tar fram elementkrafterna.
for i=1:26
	es((2*i-1):2*i,:)=beam3s(ex(i,:),ey(i,:),ez(i,:),...
         						 eo(i,:),ep(i,:),ed(i,:),a);
end;

% Plottar resultatet.
figure(1);
title('Olles stol');
eldraw3(ex,ey,ez,[1 2 1],Edof(:,1));
%eldisp3(ex,ey,ez,ed,[1 4 0],1);
axis([-0.1 0.6 -0.1 0.6 -0.1 1.0]);