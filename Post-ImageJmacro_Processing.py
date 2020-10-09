#!/usr/bin/python3

try:
    import sys
    import matplotlib.pyplot as plt
    import pandas
    from os import listdir
    from os.path import isfile, join
    from tkinter import filedialog
    import argparse
except:
    print("[!] You are missing required dependancies!")

if sys.version_info[0] < 3:
    print("[!] Please run using Python 3")
    sys.exit()

ap = argparse.ArgumentParser()
ap.add_argument("-p","--path", required=False, help="Path to directory containing CSV files")
args = vars(ap.parse_args())

def browseFiles():
    if args['path'] == None:
        filename = filedialog.askdirectory(initialdir = ".", title = "Select folder with CSV output data")
    else:
        filename = str(args['path'])
        
    return filename

def GET_CSV_FILES(P):
    file_list = [f for f in listdir(P) if isfile(join(P, f))]
    csv_list = []
    for file in file_list:
        if ".csv" in str(file).lower():
            csv_list.append(file)

    return(csv_list)      

def COMBINE(path,csv_list):

    l = len(csv_list)

    DATA = [None]*l; NAMES = [None]*l

    for i in range(0,l):
        NAMES[i] = csv_list[i][:-9]
        values = pandas.read_csv(path+csv_list[i])["Mean"].values.tolist()
        
        print('''Saving data for "{}"'''.format(csv_list[i]))
        DATA[i] = values
        
        j=0

        while(j <= i-1):
            if i>=1 and NAMES[i] == NAMES[j]:
                print('''Found "{}". Adding to "{}"'''.format(csv_list[i],NAMES[j]))
                DATA[j] = DATA[j] + values
                DATA[i] = None; NAMES[i] = None
                break
            j = j+1

    NAMES = [i for i in NAMES if i]
    DATA = [i for i in DATA if i]
    print("\n")

    DATA_AS_DF = (pandas.DataFrame(DATA).transpose().rename(columns={i : NAMES[i] for i in range(0,len(NAMES))})).iloc[1:]
    
    return(NAMES,DATA,DATA_AS_DF)

def save_intense(DATA):
    a = input("Do you want to save this data? (y/n) >> ").lower()

    if a == "yes" or a == "y":
        try:
            DATA.to_csv("./output_intensity.csv",index=False)
            print("[+] Saved as output_intensity.csv! ")
        except:
            print("[!] Error saving data... Sorry dude...")

def graph_intense(NAMES,DATA):
    try:
        _, ax1 = plt.subplots()
        ax1.set_title("Box Plot of Nuclear pSMAD1/5 intensity")
        ax1.boxplot(DATA,labels=NAMES)
        plt.show()
    except:
        print("\n[!] Error! Cannot plot data...")

def calc_percent(NAMES,DATA,THRESHOLD):
    N = []

    try:
        print("\nDATA:\n")

        for i in range(0,len(NAMES)):
            n_above_thresh = 0; total =0
            for val in DATA[i]:
                total = total + 1
                if val > THRESHOLD:
                    n_above_thresh = n_above_thresh + 1
            
            percentage = (n_above_thresh/total)*100
            N.append(percentage)
            print(NAMES[i]+" : "+str(percentage)+"%")
        return N
    except:
        print("[!] Error: Failed to calculate % positive cells")
        quit()

def save_percent(NAMES,data):

    a = input("Do you want to save this data? (y/n) >> ").lower()
    if a == "y" or a == "yes":
        with open ('./output_%postive.csv','w') as f:
            str_names = str(NAMES)[1:-1].replace("'","")
            str_data = str(data)[1:-1]
            file_data = str_names+"\n"+str_data
            f.write(file_data)
            f.close()
        print("[+] Saved data to output_%postive.csv!")

def graph_percent(NAMES,data):
    try:
        _, ax = plt.subplots()
        ax.bar(NAMES,data)
        ax.set_ylabel("% Positive Cells")
        plt.show()
    except:
        print("[!] Unable to plot :(")

def main():
    try:
        option = input("\nEnter [1] for Nuclear Intensity per cell or enter [2] to get % Postive cells of Nuclear Expression >> ")
        print("\n")
        path = str(browseFiles()) + "/"
        
        CSVS = GET_CSV_FILES(path)
        
        if len(CSVS) == 0 or path == None:
            print("[!] Error: No CSV files found in the directory provided or directory is invalid")
            print("Please try again!")
            quit()


        NAMES,D,DATA = COMBINE(path,CSVS)

        if str(option) == "1":
            print(DATA)
            save_intense(DATA)
            g = input("Do you want to plot this? (y/n) >> ").lower()
            if g == "y" or g == "yes":
                graph_intense(NAMES,D)
            else:
                quit()
        elif str(option) == "2":
            print(DATA)
            save_intense(DATA)
            def thresh():
                try:
                    threshold = int(input("\nEnter a numberic threshhold for postive/negative cells >> "))
                except KeyboardInterrupt:
                    quit()
                except:
                    print("[!] Error: Please Enter an integer!")
                    threshold = thresh()
                return threshold
            threshold = thresh()
            DATA = calc_percent(NAMES,D,threshold)
            
            save_percent(NAMES,DATA)
            g = input("Do you want to plot this? (y/n) >> ").lower()
            if g == "y" or g == "yes":
                graph_percent(NAMES,DATA)
            else:
                quit()

        else:
            print("[!!] Invalid Option")
    except KeyboardInterrupt:
        quit()

if __name__ == "__main__":
        main()