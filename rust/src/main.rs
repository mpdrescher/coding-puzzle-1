use std::io;
use std::thread;

struct Sample
{
	no: usize,
	string: String
}

fn main() 
{
	let mut cases_str = String::new();
	let mut samples_str = String::new();
	let _ = io::stdin().read_line(&mut cases_str);
	let _ = io::stdin().read_line(&mut samples_str);
	let cases = cases_str.trim().parse::<usize>().unwrap();
	let samples = samples_str.trim().parse::<usize>().unwrap();
	let lines = cases * samples;
	let mut sample_vec = Vec::new();
	for i in 0..lines
	{
		let mut cur_line = String::new();
		let _ = io::stdin().read_line(&mut cur_line);
		sample_vec.push(Sample{no: i, string: cur_line.trim().to_owned()});
	}

	let mut thread_handles = Vec::new();
	for item in sample_vec
	{
		thread_handles.push(thread::spawn(move || { 
			let mut well_formed = true;
			let mut stack: Vec<usize> = Vec::new();
			for ch in item.string.chars()
			{
				let assoc_number = match ch
				{
					'(' | ')' => 1,
					'[' | ']' => 2,
					'{' | '}' => 3,
					_ => 4
				};
				if assoc_number == 4
				{
					well_formed = false;
				}
				if ch == '(' || ch == '[' || ch == '{'
				{
					if stack.is_empty()
					{
						if ch != '('
						{
							well_formed = false;
						}
					}
					else
					{
						match (stack.last().unwrap().to_owned(),assoc_number)
						{
							(1,3) => {},
							(2,1) => {},
							(2,2) => {},
							(2,3) => {},
							(3,2) => {},
							_ => {well_formed = false;}
						}
					}
					stack.push(assoc_number);
				}
				else 
				{
				    if stack.is_empty()
				    {
				    	well_formed = false;
				    }
				    else 
				    {
				        if stack.pop().unwrap() != assoc_number
				        {
				        	well_formed = false;
				        }
				    }
				}
			}
			if !stack.is_empty()
			{
				well_formed = false;
			}
			println!("{}:{}", item.no+1, well_formed);
		}));
	}

	for thread in thread_handles
	{
		let _ = thread.join();
	}
}