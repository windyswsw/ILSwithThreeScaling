## Iterated Local Search (ILS) based resource allocation approach for VNFs with three scaling mechanisms:

Network Function Virtualization (NFV) is a promising technology that proposes to move network-based services from dedicated hardware middle-boxes to software running on commodity servers: Virtualized Network Functions (VNFs). As such, NFV brings the possibility of outsourcing enterprise Network Function (NFs) processing to the cloud. When an enterprise outsources its NFs to a Cloud Service Provider (CSP), the CSP is responsible for deciding: (1) where initial Virtual NFs (VNFs) should be instantiated, and (2) what, when and where additional VNFs should be instantiated to satisfy the traffic changes (scaling) with minimal impact on network performances. 

In this project we introduce an Iterated Local Search (ILS) based resource allocation approach for VNFs that supports three scaling methods. ILS based resource allocation approach, starts with an initial solution (based on a simple Depth First Search: DFS or random approach) and continue with a selected scaling method that supports dynamic traffic changes over the time. There are three scaling choices: (1) vertical scaling, (2) migration scaling and (3) horizontal scaling. The ILS approach tries to maximize the accepted bandwidth requests while ensuring the delay experienced by each packet of the accepted requests does not exceed its relative deadline. 

This repository contains programs for the ILS based resource allocation algorithm.

## Project Structure:

1. ILSwithDFSandThreeScaling folder: Initial solution based on DFS (Source file, Header files and Input data)

2. ILSwithRandomandThreeScaling folder: Initial solution based on random (Source file, Header files and Input data)

## Guidelines to use the data and programs in the repository:

There are two ways that this repository can be useful for anyone looking for resource allocation approaches for VNFs.

1. Directly use the program with the given data set (in inputs folder) by running the source file

2. Use the programs with your own data sets (We have a data set generation repository at https://github.com/windyswsw/DataForNFVSDNExperiments) 

## Inputs to the program:

1. Topology structure: Paths between each source and destination server pair (Paths.txt), Entering (InPaths.txt) and exiting path (OutPaths.txt) for each server
2. Topology capacities: Link capacities (Links.txt), Server capacities (Servers.txt)
3. VNF profile: Server and bandwidth requirement of VNFs (NF.txt)
4. Policy details: VNF chain information (Policy.txt)
5. Scaling requirments over the time: VNFs that needs to be scaled out/in over the time (ScalePattern.txt)

## Running the program:

The user can select the scaling approach (V for vertical, U for migration and I for horizontal) and pass it as a commandline argument when running the program.

