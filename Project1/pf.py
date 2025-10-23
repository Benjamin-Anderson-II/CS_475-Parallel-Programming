import csv


# Get's the time in seconds from the megaTrialsPerSecond
def getTime(numTrials, megaTrialsPerSecond):
    return numTrials / (1_000_000 * megaTrialsPerSecond)


# Gets the speed up given the two times
def getSpeedUp(time1, timeN):
    return time1 / timeN


# Calculates the Partial Fraction given the speed up and # of cores
def getPF(speedUp, cores):
    return (cores / (cores-1)) * ((speedUp - 1) / speedUp)


# Open the file
file = open("proj1.csv")
csvreader = csv.reader(file)

# Read the file's contents
rows = []
for row in csvreader:
    row.pop(2)                              # Remove percentages
    rows.append(list(map(str.strip, row)))  # Remove leading and trailing spaces
file.close()


# Remove all cores except 1 and 6
rows_clean = list(filter(lambda x: x[0] == '1' or x[0] == '6', rows))

# Get Times from megaTrialsPerSecond
times = [getTime(float(x[1]), float(x[2])) for x in rows_clean]

# Get Speedups from from times
speedUps = []
for i in range(8):
    speedUps.append(getSpeedUp(times[i], times[i+8]))  # The two data sets are 8 away

# Get partial fraction from speedups
pfs = [getPF(x, 6) for x in speedUps]

print("Partial Fractions:\n")
print(pfs)
