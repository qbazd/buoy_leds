import pandas as pd
df = pd.read_excel (r'sekwencje.xlsx',usecols=[0,1,2,3,4])



led_types = {
	'Q6_LFI_15s' : [300,700,300,700,300,700,300,700,300,700,300,700,2000,7000],
	'Mo_A_10s' : [500,500,1500,7500,0,0,0,0,0,0,0,0,0,0],
	'FI_3s' : [1000,2000,0,0,0,0,0,0,0,0,0,0,0,0],
	'LFI_10s' : [3000,7000,0,0,0,0,0,0,0,0,0,0,0,0],
	'Iso_10s' : [5000,5000,0,0,0,0,0,0,0,0,0,0,0,0],
	'Q' : [500,500,0,0,0,0,0,0,0,0,0,0,0,0],
	'Oc_10s' : [6000,4000,0,0,0,0,0,0,0,0,0,0,0,0],
	'Iso_4s' : [2000,2000,0,0,0,0,0,0,0,0,0,0,0,0],
	'DIR' : [5000,1,0,0,0,0,0,0,0,0,0,0,0,0],
	'Q_3_10s' : [300,700,300,700,300,7700,0,0,0,0,0,0,0,0],
	'Oc_WRG_4s' : [2500,1500,0,0,0,0,0,0,0,0,0,0,0,0]
}
colors = {
	'R' : [255,0,0],
	'G' : [0,255,0],
	'Y' : [255,150,0],
	'W' : [255,255,150]
}
num_of_leds = 132;
f = open("led_list.txt", 'w')

latarns = {
'#Jastarnia' : [],
'#Kaszyca' : [],
'#Hel' : [],
'#Gdansk' : [],
'#Sopot' : [],
'#Krynica' : [],
'#Shchuukinskiy' : [],
'#Obzornyy' : [],
'#Taran' : [],
'#Rozewie' : []
}

# f.write("Now the file has more content!")

def generate_bouys():
	f.write('buoy buoyList[BUOYS] = {')
	for row in df.values:
		if(row[4][0] == '#'):
			latarns[row[4]].append(row)
		else:
			f.write('{'+str(row[0])+','+str(row[1])+','+str(row[2])+','+str(colors[row[3]]).replace('[','{').replace(']','}')+','+str(sum(map(lambda x : x!= 0, led_types[row[4]]))-1)+',0,'+str(led_types[row[4]]).replace('[','{').replace(']','}')+',0},'+'\n')
	f.write('};')
# def generate_latarns():
# 	for row in df.values:
# 		if(row[4][0] == '#'):
# 			latarns[row[4]].append(row)
generate_bouys()

print(latarns)
f.close()