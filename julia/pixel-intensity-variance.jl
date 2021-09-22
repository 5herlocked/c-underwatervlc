using Plots, CSV, DataFrames, Query, Statistics, StatsPlots
gr()

# load all the available data
begin
    data_location = String(pwd()) * "\\julia\\test-set\\"
    available_files = readdir(data_location)
end

begin
    temp_files = available_files |> @filter(occursin("c", split(_, '.')[1])) |> collect
    
    temp_dF = DataFrame(CSV.File(data_location * temp_files[1]))

    temp_list_off = temp_dF |> @filter(_.bit == 0) |> DataFrame
    temp_list_on = temp_dF |> @filter(_.bit == 1) |> DataFrame

    plotandExport(temp_list_off, "Pixel Intensities OFF, 28c", "Pixel Intensity OFF 28c")
    plotandExport(temp_list_on, "Pixel Intensities ON, 28c", "Pixel Intensity ON 28c")
end

begin
    salinity_files = available_files |> @filter(occursin("pp", split(_, '.')[1])) |> collect

    if size(salinity_files)[1] <= 1
        return
    end

    salinity_dF = DataFrame(CSV.File(data_location*salinity_files[1]))

    salinity_list_off = salinity_dF |> @filter(_.bit == 0) |> DataFrame
    salinity_list_on = salinity_dF |> @filter(_.bit == 1) |> DataFrame 

    plotandExport(salinity_list_off, "Pixel Intensity OFF")
end

function plotandExport(df, title, fn)
    temp = boxplot(
        title=title,
        label=["Blue" "Red" "Green"],
        ["Blue" "Red" "Green"],
        [df.blue df.red df.green]
    )

    savefig(temp, pwd()*"\\julia\\"*fn*".svg")
end