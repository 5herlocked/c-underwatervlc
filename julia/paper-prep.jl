using Plots, CSV, DataFrames, Query
gr()

data_source = DataFrame(CSV.File("C:\\Users\\camv7\\source\\c-underwatervlc\\julia\\Average Pixel intensities.csv"))

begin
	temp_dataset_off = data_source |> @filter(occursin("c", _.dataset) && _.led == "off") |> DataFrame
	
	temp_dataset_on = data_source |> @filter(occursin("c", _.dataset) && _.led == "on") |> DataFrame
	
	plotAndExport(temp_dataset_off, "Temperature, LED OFF vs. Pixel Intensities", "C:\\Users\\camv7\\Downloads\\TempScatterOff.svg")
	plotAndExport(temp_dataset_on, "Temperature, LED ON vs. Pixel Intensities", "C:\\Users\\camv7\\Downloads\\TempScatterOn.svg")
end

begin
	salinity_dataset_off = data_source |> @filter(occursin("pp", _.dataset) && (_.led == "off")) |> DataFrame
	salinity_dataset_on = data_source |> @filter(occursin("pp", _.dataset) && (_.led == "on")) |> DataFrame

	plotAndExport(salinity_dataset_off, "Salinity, LED OFF vs. Pixel Intensities", "C:\\Users\\camv7\\Downloads\\SalinityScatterOff.svg")
	plotAndExport(salinity_dataset_on, "Salinity, LED ON vs. Pixel Intensities", "C:\\Users\\camv7\\Downloads\\SalinityScatterOn.svg")
end

begin
	ph_dataset_off = data_source |> @filter(occursin("ph", _.dataset) && (_.led == "off")) |> DataFrame
	ph_dataset_on = data_source |> @filter(occursin("ph", _.dataset) && (_.led == "on")) |> DataFrame

	plotAndExport(ph_dataset_off, "pH, LED OFF vs. Pixel Intensities", "C:\\Users\\camv7\\Downloads\\pHScatterOff.svg")
	plotAndExport(ph_dataset_on, "pH, LED ON vs. Pixel Intensities", "C:\\Users\\camv7\\Downloads\\pHScatterOn.svg")
end

begin
	turbid_dataset_off = data_source |> @filter(occursin("g", _.dataset) && (_.led == "off")) |> DataFrame
	turbid_dataset_on = data_source |> @filter(occursin("g", _.dataset) && (_.led == "on")) |> DataFrame

	plotAndExport(turbid_dataset_off, "Turbidity, LED Off vs. Pixel Intensity", "C:\\Users\\camv7\\Downloads\\TurbidScatterOff.svg")
	plotAndExport(turbid_dataset_on, "Turbidity, LED On vs. Pixel Intensity", "C:\\Users\\camv7\\Downloads\\TurbidScatterOn.svg")
end

function plotAndExport(dataset, title, filename)
	temp = scatter(
		dataset.dataset,
		[dataset.blue, dataset.red, dataset.green, dataset.grey],
		label=["Blue" "Red" "Green" "Grey"],
		title=title,
		xlabel="Dataset",
		ylabel="Pixel Intensity (0-255)"
	)

	savefig(temp, filename)
end