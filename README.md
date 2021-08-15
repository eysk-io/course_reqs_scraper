# Multithreaded UBC Course Info Web Scraper

A simple web scraper to retrieve UBC course information as provided by http://www.calendar.ubc.ca/vancouver/courses.cfm?page=name and saved to a MongoDB database.

## Getting Started

The program supports building and running on the `Linux` operating system. As well, `make` and `gcc` should be installed. One way to install `make` and `gcc` on your `Linux` machine:

```shell
$ sudo apt-get install make
$ sudo apt-get install gcc
```

In addition, `mongo-c-driver` is required to run the program. For more information and installation: https://github.com/mongodb/mongo-c-driver.

Finally, an environment variable - `$MONGO_URI` - with your MongoDB database URI should be set for use by the program. For example, if your MongoDB URI is `mongodb+srv://user:password.mongodb.net`, your environment variable should be set as `MONGO_URI=mongodb+srv://user:password.mongodb.net`. This is the database in which all course data will be saved. 

By default, the database name is: `course_reqs_db`. The collection name is `courses`. This can be found in `subject_page_scraper.c`. 

### Building & Running the Program

A Makefile is included to simplify the build process. In short, to build and run the program: 

```shell
git clone git@github.com:eyskim/course_reqs_scraper.git
cd course_reqs_scraper/
make clean
make
./main
```

## Features

Overall, the program is broken down into three main steps:

1. Retrieve links to each course subject page from http://www.calendar.ubc.ca/vancouver/courses.cfm?page=name.

2. Retrieve each course and its information from each subject page.
    - ex. http://www.calendar.ubc.ca/vancouver/courses.cfm?page=name&code=ADHE for all ADHE course information.

3. Save all course data to a MongoDB database.

To speed up the process to retrieve and save course information, the program uses a thread pool - `tpool.c` - to parallelize processes. Specifically, saving each subject's courses is performed by each thread.

### Upcoming

- Correctly parsed pre/co-requisites and equivalent course information

## Configuration

Here you should write what are all of the configurations a user can enter when
using the project.

#### Argument 1
Type: `String`  
Default: `'default value'`

State what an argument does and how you can use it. If needed, you can provide
an example below.

Example:
```bash
awesome-project "Some other value"  # Prints "You're nailing this readme!"
```

#### Argument 2
Type: `Number|Boolean`  
Default: 100

Copy-paste as many of these as you need.

## Contributing

If you'd like to contribute, please fork the repository and use a feature branch. Pull requests are welcome.

## Links

- Repository: https://github.com/eyskim/course_reqs_scraper/
- Issue tracker: https://github.com/eyskim/course_reqs_scraper/issues
  - In case of sensitive bugs like security vulnerabilities, please contact `erickim195@gmail.com` directly instead of using issue tracker. Thank you for your efforts to improve the security and privacy of this project!
- Related projects (coming soon):

## Licensing

The code in this project is licensed under MIT license.