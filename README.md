# MICROelec_W
MicroElec physics list for dust in plasma

*******************
RUNNING SIMULATIONS
*******************

The files that i mention should be the only ones you need to modify when running simulations.

1) construction.cc
Here you change the dust geometry i.e radius, material etc as well as the same for the world volume and spherical detector encasing the dust particle 





2) G4MicroElecSurface.cc
Here is where i modified the work function of W for the transmission coefficient calculation, specifially in lines 558, 559:

				G4double thresholdNew_surface = (matNew == "G4_W") ? 9.5 * eV : postStepWF->second;
				G4double thresholdOld_surface = (matOld == "G4_W") ? 9.5 * eV : preStepWF->second;




And lines 682,683:

			G4double thresholdNew = (matNew == "G4_W") ? 9.5 * eV : postStepWF->second;
			G4double thresholdOld = (matOld == "G4_W") ? 9.5 * eV : preStepWF->second;

Here you just change the value if needed. 








3) sim.cc

At line 56:    std::ifstream macroList("macros150/macro_list.txt");

you must make sure the name of the macro file youre using is correct. I know this is tedious but it is relatively easy to change in the cluster quickly.






4) physicsMircoElecNew.cc

yes, i know the name is misspelled lol

Here is where i modified the processes and tweaked the thresholds in which each model is enabled.

I should have all standard models that Opt4 uses enabled above 7kev (or whichever energy ranges Opt4 uses abovre 7kev). For example:

  G4UrbanMscModel* msc = new G4UrbanMscModel();
  msc->SetActivationLowEnergyLimit(7 * keV);
  em_config->SetExtraEmModel("e-", "msc", msc, "Target", 7 *keV, 100 * MeV);

  Here you just change the 7kev for whichever value you want.



  Same applies to the MicroElec models i have enabled before 7 kev:

  mod = new G4MicroElecElasticModel_new();
  em_config->SetExtraEmModel("e-", "e-_G4MicroElecElastic", mod, "Target", 0.1 * eV, 7 * keV);


Other than the processes for Opt4, i have not added anything else, and all of the commented out things were already there in the code for MicroElec. I have kept all commented out things throughout in case they may be needed in the future.











*******************************************************************
OUTPUT
*******************************************************************

You will get two folders named 'output_hits' and 'output_stepping'. Only care about the output_stepping!!!! I need to get rid of the output_hits but never got around to it.

the output_stepping will show all files and will include:

#column double Ekin
#column int ParticleID
#column int EventID
#column double z coordinate
#column int ParentID
#column int isSEE
#column int isBSE
#column int isTEY

8788.31,1,880,0.0153865,0,0,1,1









*************************************************
MATLAB SCRIPTS
*************************************************
1) macros.m

script to generate the macro files i used. you input the parameters and choose the name of the macro folder. this is crucial as it will be what you have to change in the sim.cc file to the correct name 






2) PostProcess.m

This is the first script in the pipeline to use. You simply paste the path of the 'output_stepping' file containing all of the output files and run the script.

A new folder named 'combined_results' will show all the output files that have been combined by their threads so each parameter combination is just one big file. 

This folder will also include a file named 'escape_probabilities.csv' which we will use next.





3) angle_average.m

The next script is this one where again you simply paste the path file of the escape_probabilities.csv into the beginning of the script and run.

A new file named averaged_escape_probabilities.csv will be created which contains the angle averaged escape yields for each parameter combination 




3) weighting.m 
the final script. We paste the averaged_escape_probabilities.csv in line 42:

data = readtable('/Volumes/LaCie/MSc Nuclear Engineering/Thesis/DATAA/0.3um/output/averaged_escape_probabilities.csv');

and also the escape_probabilities.csv in line 251 for angular analysis:

raw_data = readtable('/Volumes/LaCie/MSc Nuclear Engineering/Thesis/DATAA/0.3um/output/combined_results/escape_probabilities.csv');

(the latter step isnt necesary but it will analysis the angular dependence). You choose the paramater combination at line 245:

selected_energies = [1, 5, 11, 15 , 17]; % keV 
%selected_radii = [4.9, 4.92, 4.94, 4.95, 4.96, 4.97, 4.98, 4.99 5]; % μm 
selected_radii = 0; % μm 

The output will show figures for the radial dependence, angular and energy dependence.





4) fitting_v2.m

script used to fit the data to the models. very simple, just input the escape data for a certain size dust and run.






5) chargingv2.m

script for calcualting the self charging rates by integrating the models obtained in the script prior.

Here input the fitting parameters for each dust size at line 24. There is a lot of radnom analysis that i played with but the output essentially gives you the self charging rates for C/s and e/s as well as some analysis on the parameters a,b and c to look at the trends.

The data at the bottom is from the french guys paper who used Opt4, just to compare the charging rates to them.






Finally, once build i simply had the OUTPUT file where in each folder i put my macros and batch files, i had my vis.mac file and then the necessary cmake files etc as you know.