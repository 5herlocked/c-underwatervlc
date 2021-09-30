using Plots, CSV, Query, DataFrames
gr()

data_location = "E:\\csvs\\"
available_files = readdir(data_location)

function getTransmitterPattern(transmitterVector::Vector{Int16}, precision, ratio)::String
	pattern = ""
	
	for i in range(1, precision; step=1)
		bit = transmitterVector[i]
		for repeat in range(1, ratio; step=1)
			pattern *= string(bit)
		end
	end
	
	return pattern
end

function getReceiverString(receiverVector::Vector{Int16})::String
	receiver = ""
	
	for i in receiverVector
		receiver *= string(i)
	end
	
	return receiver
end

function getVectorFromFile(file::CSV.File)::Vector{Int16}
	fileVector = Vector{Int16}()
	
	for log in file
		push!(fileVector, log.bit)
	end
	
	return fileVector
end

function getBER(sent, received, ratio)::Float64
	success = 0
	rec_counter = 0
	for b in sent
		iSucc = 0
		for i in 1:ratio
			rec_counter += 1
			if received[rec_counter] == b
				iSucc += 1
			end
		end
		
		if iSucc >= ratio/2
			success += 1
		end
	end
end

begin
    temp_files = available_files |> @filter(occursin("c", split(_, '.')[1])) |> collect
    transmitter_files = temp_files |> @filter(occursin("transmitter", split(_, '.')[1])) |> collect

    matched_files = Set{String}()
    
    for file_name in temp_files
        if occursin("transmitter", file_name)
            continue
        end

        dataset_specifier = split(split(file_name, ".")[1], "_")
        
        ratio = 0
        
        if dataset_specifier[3] == "25Hz"
            ratio = 4
        elseif dataset_specifier[3] == "50Hz" || dataset_specifier[3] == "15Hz" || dataset_specifier[3] == "30Hz"
            ratio = 2
        elseif dataset_specifier[3] == "5Hz"
            ratio = 3
        elseif dataset_specifier[3] == "1Hz"
            ratio = 100
        end

        if dataset_specifier[3] in matched_files
            continue
        end
        push!(matched_files, dataset_specifier[3])
        transmitter = transmitter_files |> @filter(occursin(dataset_specifier[3], _) && occursin(dataset_specifier[1], _)) |> collect
        
        transmitter = CSV.File(data_location*transmitter[1])
        receiver = CSV.File(data_location*file_name)

        transmitter_vec = getVectorFromFile(transmitter)

        transmitter_pattern = getTransmitterPattern(transmitter_vec, 3, ratio)
    end
end
