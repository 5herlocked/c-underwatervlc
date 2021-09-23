using Plots, CSV, DataFrames, Query, Statistics, StatsPlots
gr()

# load all the available data
begin
    data_location = String(pwd()) * "\\test-set\\"
    available_files = readdir(data_location)
end

begin
    temp_files = available_files |> @filter(occursin("c", split(_, '.')[1])) |> collect

    for file in temp_files
        title = split(file, '.')[1]
        temp_dF = DataFrame(CSV.File(data_location * file))

        temp_list_off = temp_dF |> @filter(_.bit == 0) |> DataFrame
        temp_list_on = temp_dF |> @filter(_.bit == 1) |> DataFrame

        plotandExport(temp_list_off, title*"OFF", title*"OFF")
        plotandExport(temp_list_on, title*"ON", title*"ON")
    end
end

begin
    salinity_files = available_files |> @filter(occursin("pp", split(_, '.')[1])) |> collect

    if size(salinity_files)[1] <= 1
        return
    end

    for file in salinity_files
        title = split(file, '.')[1]
        temp_dF = DataFrame(CSV.File(data_location*file))

        temp_list_off = temp_dF |> @filter(_.bit == 0) |> DataFrame
        temp_list_on = temp_dF |> @filter(_.bit == 1) |> DataFrame

        plotandExport(temp_list_off, title*"OFF", title*"OFF")
        plotandExport(temp_list_on, title*"ON", title*"ON")
    end
end

begin
    ph_files = available_files |> @filter(occursin("ph", split(_, '.')[1])) |> collect

    for file in ph_files
        title = split(file, '.')[1]
        temp_dF = DataFrame(CSV.File(data_location*file))

        temp_list_off = temp_dF |> @filter(_.bit == 0) |> DataFrame
        temp_list_on = temp_dF |> @filter(_.bit == 1) |> DataFrame

        plotandExport(temp_list_off, title*"OFF", title*"OFF")
        plotandExport(temp_list_on, title*"ON", title*"ON")
    end
end

begin
    turbid_files = available_files |> @filter(occursin("g", split(_, '.')[1])) |> collect

    for file in turbid_files
        title = split(file, '.')[1]
        temp_dF = DataFrame(CSV.File(data_location*file))

        temp_list_off = temp_dF |> @filter(_.bit == 0) |> DataFrame
        temp_list_on = temp_dF |> @filter(_.bit == 1) |> DataFrame

        plotandExport(temp_list_off, title*"OFF", title*"OFF")
        plotandExport(temp_list_on, title*"ON", title*"ON")
    end
end

function plotandExport(df, title, fn)
    temp = boxplot(
        title=title,
        label=["Blue" "Red" "Green"],
        ["Blue" "Red" "Green"],
        [df.blue df.red df.green],
        ylims = (0, 255)
    )

    savefig(temp, pwd()*"\\export\\"*fn*".svg")
end