#!/usr/bin/Rscript

#install.packages("beeswarm")
library(beeswarm)

#Please make sure if you are doing multiple replicates the ending of each replicate only differs by a number from macro

#For examples: Labels should look like "NT VEH 100X_1.oir.csv, NT VEH 100X_2.oir.csv" The "1" and "2" is used by the program to merge the groups.
#IF data is labeled in correctly it will not properly merge replicates, and every images will be plotted seperatly


path <- "C:/Users/sx/Desktop/10.14.2020/SMURF_DATA/"   #change this for the folder where your CSVs are exported
Get_Threshold <- FALSE   #if you want to calculate the threshold make this "TRUE". Else change to "FALSE" 
threshold <- 500        #Change this number to be the threshhold for %positive cells.


setwd(path)


GET_CSV <- function(path) #Gets all CSV files in specified folder.
{
  CSV_FILES <- list()
  files<-list.files(path) 
  
  for(i in 1:length(files)) 
  { 
    if(substr(files[i], nchar(files[i])-3, nchar(files[i])) == ".csv")
    {
      CSV_FILES <- c(CSV_FILES,files[i])
    }
  }
  return(CSV_FILES)
}


COMBINE <- function(PATH,CSV_LIST) #Combines the data from csv files into replicates
{
  l = length(CSV_LIST); DATA <- list(); NAMES=list(); MASTER_CSV=list()
  
  for(i in 1:l)
  {
    NAMES <- c(NAMES,substr(CSV_LIST[i],1,nchar(CSV_LIST[i])-9))
    
    j=1; found = FALSE
    
    while(j <= i-1)
    {
      if(i > 1 && as.character(NAMES[i]) == as.character(NAMES[j]))
      {
        print(paste("Found",CSV_LIST[i],": Saving to --> ",NAMES[j])); found = TRUE
        m <- c(read.csv(CSV_LIST[[i]])$Mean , read.csv(CSV_LIST[[j]])$Mean)
        MASTER_CSV[as.character(NAMES[i])] <- list(m)
        break
      }
      j = j+1
    }
    if(!found)
    {
      print(paste("Making",NAMES[i],"condition"))
      n <- read.csv(CSV_LIST[[i]])$Mean
      MASTER_CSV[as.character(NAMES[i])] <- list(n)
    }
    
  }
  return(MASTER_CSV)
}


GET_PERCENT <- function(DATA,threshold) 
{
  threshold = as.numeric(threshold)
  n_over = 0; n_total = 0; i=0;
  percents <- list(); names <- names(DATA)
  
  for(condition in DATA)
  {
    for(i in 1:length(condition))
    {
      if(as.numeric(condition[1]) > threshold)
      {
        n_over = n_over + 1
      }
      n_total = n_total + 1
    }
    i=i+1
    percent_pos = (n_over/n_total)*100
    percents[as.character(names[i])] <- percent_pos
  }
  percents <- data.frame(percents)
  colnames(percents) <- names
  return(percents)
}


CSV_FILES <- GET_CSV(path = path)
MASTER_CSV <- COMBINE(PATH = path, CSV_LIST = CSV_FILES)

print("Done calculating!")
print("Plotting and saving...")

beeswarm(MASTER_CSV, corral = 'omit', pch =20, col = 1, ylab = "Nuclear Intensity")
bxplot(MASTER_CSV, add = T)

if(Get_Threshold)
{
  THRESHOLD_DATA <- GET_PERCENT(MASTER_CSV,threshold = threshold)
  barplot(as.vector(unlist(unname(THRESHOLD_DATA))), ylab = paste("Percent Cells over ",threshold))
  axis(1,a=1:length(THRESHOLD_DATA), labels = names(THRESHOLD_DATA))
  write.csv(THRESHOLD_DATA, file = "Threshold_DATA.csv")
}

max = 0
for(i in 1:length(MASTER_CSV))
{
  if(length(unlist(MASTER_CSV[i])) > max)
  {
    max = length(unlist(MASTER_CSV[i]))
  }
}

MASTER_df <- as.data.frame(matrix(nrow = max, ncol = length(MASTER_CSV), byrow=T))
for(j in 1:length(MASTER_CSV))
{
  MASTER_df[j] <- c(unname(unlist(MASTER_CSV[j])), rep(NA, max - length(unlist(MASTER_CSV[j]))))
}

colnames(MASTER_df) <- names(MASTER_CSV)

write.csv(MASTER_df, file = "Merged_Data.csv")

print(paste("Done!, Check",path," for you data!"))

