# realFluidFoam-8

## General Information
A low Mach number solver for simulations of turbulent flows at trancritical and supercritical conditions in OpenFOAM 8.0. This solver is developed based on the real-fluid based _thermophysicalModels_ libary [1] and _reactingFoam_ solver. In this solver, a pressure-based solution method with a modified PIMPLE algorithm [2] is employed to improve the stability while a fast and robust coupling Newton-Bisection algorithm is utilized to guarantee the convergency of fluid flow simulations under transcritical and supercritical conditions.

## Real-fluid models can be used with this solver
- Modified Soave-Redlich-Kwong (SRK) model for equation of state [3, 4].
- Peng-Robinson (PR) model for equation of state [5]
- JANAF-based model for real-fluid thermodynamic properties.
- Chung's model (1988) for dynamic viscosity and thermal conductivity [6].

## Installation
- Since this solver is developed based on OpenFOAM 8.0 in Linux operating systems, the complete installation of OpenFOAM 8.0 framework is required. 
- Prepare a directory on your system, e.g., _yourDirectory_:

		mkdir ~/OpenFOAM/yourDirectory/
		cd ~/OpenFOAM/yourDirectory/	
- Download source files using git: 

		git clone https://github.com/danhnam11/realFluidFoam-8.git

- Specify the path of your _src_ directory to an environment variable, _LIB_rfFoam8_SRC_. For example:

		echo "export LIB_rfFoam8_SRC=~/OpenFOAM/yourDirectory/realFluidFoam-8/src/" >> ~/.bashrc
		source ~/.bashrc
- To compile the necessary libraries and solver, go to _realFluidFoam-8_ directory and run the _Allwmake_ script:

		cd ~/OpenFOAM/yourDirectory/realFluidFoam-8/
		./Allwmake

- Now all necessary libraries inlcuding the real-fluid based _thermophysicalModels_ and _realFluidFoam_ solver are stored at _$FOAM_USER_LIBBIN_ and _$FOAM_USER_APPBIN_ directory. These newly compiled libraries and solver are ready to be used.

- To remove all compiled libraries and solvers, go to _realFluidFoam-8_ directory and run the _Allwclean_ script:

		cd ~/OpenFOAM/yourDirectory/realFluidFoam-8/
		./Allwclean

## Using realFluidFoam solver 
- Upon completing the compilation process, the solver can be utilized with different real-fluid models by simply typing _realFluidFoam_ in the terminal. 
- It is of importance to note that the runtime names of real-fluid thermophysical models also need to be specified correctly. For details of using real-fluid models, reader can refer to https://github.com/danhnam11/realFluidThermophysicalModels-8 
- Readers are referred to our paper for the validation of the new solver.

## Tutorials
Tutorials for LES of a cryogenic nitrogen jet is available in the _tutorial_ directory.

	cd ~/OpenFOAM/yourDirectory/realFluidFoam-8/tutorials/

## Authors 
This package was developed at Clean Combustion & Energy Research Lab., Dept. of Mech. Engineering, Ulsan National Institute of Science and Technology (UNIST), Korea (Prof. C.S. Yoo: https://csyoo.unist.ac.kr/). If you publish results that are obtained using this package, please cite our papers as follows:
- D. N. Nguyen, C. S. Yoo, realFluidFoam: A low Mach number solver for simulations of turbulent flows at transcritical and supercritical conditions in OpenFOAM, Computer Physics Communications (2024)(submitted).
- D. N. Nguyen, K. S. Jung, J. W. Shim, C. S. Yoo, Real-fluid thermophysicalModels library: An OpenFOAM-based library for reacting flow simulations at high pressure, Comput. Phys. Commun. 273 (2022) 108264.

Contact:
- danhnam11@gmail.com or nam.nguyendanh@hust.edu.vn 

## Reference
- [1] D. N. Nguyen, K. S. Jung, J. W. Shim, C. S. Yoo, Real-fluid thermophysicalModels library: An OpenFOAM-based library for reacting flow simulations at high pressure, Comput. Phys. Commun. 273 (2022) 108264.
- [2] M. Jarczyk, M. Pfitzner, Large eddy simulation of supercritical nitrogen jets, in: 50th AIAA Aerospace Sciences Meeting Including the New Horizons Forum and Aerospace Exposition, Nashville, Tennessee, 2012. 
- [3] G. Soave, Equilibrium constants from a modified Redlich-Kwong equation of state, Chem. Eng. Sci. 27 (1972) 1197-1203.
- [4] D. Peng, D. Robinson, New two-equation of state, Ind. Eng. Chem. Fundam. 15(1976) 59-64. 
- [5] M. S. Graboski, T. E. Daubert, A modified Soave equation of state for phase equilibrium calculations. 1. Hydrocarbon systems, Ind. Eng. Chem. Process. Des. Dev. 17 (1978) 443-448.
- [6] T. C. Horng, M. Ajlan, L. L. Lee, K. E. Starling, M. Ajlan, Generalized multiparameter correlation for nonpolar and polar fluid transport properties, Ind. Eng. Chem. Res. 27 (1988) 671-679.
