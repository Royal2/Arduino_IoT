//packages:
// Iron Router
// accounts-password
// Restivus

// DataColl a Collection to hold the data value
// Meteor prefers to keep values in a Collection rather than a single variable. 
// This also allows us to store multiple values in a single location if we choose

DataColl = new Mongo.Collection('datastore'); //Initialize the collection
val0=54; //Initial value for data
time0 = new Date().getTime(); // Adding a time value. Not strictly necessary, but useful.


if (Meteor.isServer) {
  Meteor.startup(function () {
  // Clears the Collection on startup, then inserts initial value in
	DataColl.remove({}); 
	DataColl.insert(   {value: val0, timestamp: time0}   ); 
	
  });
}


//Template Helpers
if (Meteor.isClient) {
	 	   
	   
	   Template.inputData.events({
			'submit form': function(event){ //Form submission
			  event.preventDefault();
			  //the value submitted in the form
			  var valForm = event.target.inputDataVal.value;
			  //Find the ID value of the item in the collection, then overwrite 
			  // its value with the new value
			  DataID = DataColl.findOne()._id;
			  DataColl.update(DataID ,{$set: {value: valForm} });
			},
			'click .value1': function(){ //Button 1
				val1 = 5; // Value given to data if Button 1 is pressed
				DataID = DataColl.findOne()._id;
				DataColl.update(DataID ,{$set: {value: val1} });
			},
			'click .value2': function(){ //Button 2
				val2 = 10;  // Value given to data if Button 2 is pressed
				DataID = DataColl.findOne()._id;
				DataColl.update(DataID ,{$set: {value: val2} });
			}
	  
		});
	    
		//Data display template helper functions
		Template.dataTemp.helpers({
			// Find the latest (and only, if code is unmodified) value in the collection and returns it
			'lastVal' : function () {
				valRead = DataColl.findOne({},{sort: {timestamp: -1}}  ).value;
				return valRead ;
			}
		});  
    
    
        Template.alarmClock.events({
			'submit form': function(event){ //Form submission
			  event.preventDefault();
			  //the value submitted in the form
            var valForm = event.target.alarm.value;
            DataID = DataColl.findOne()._id;
			  DataColl.update(DataID ,{$set: {value: valForm} });
			},
			'click .stop': function(){
				stopVal = 0;
				DataID = DataColl.findOne()._id;
				DataColl.update(DataID ,{$set: {value: stopVal} });
			},
		});
   }
   
 //If the URL is root, displays the input form
Router.route('/', function () {
		this.layout('inputData', {  });

	});
 

 
//if the URL is /data, only displays the data for easy retrieval 
Router.route('/data', function () {
	  this.layout('dataTemp', { });


	});

//If the URL is /alarm, displays the alarm clock form
Router.route('/alarm', function () {
	  this.layout('alarmClock', { });


	});
	
	

// API commands
// Best to not modify this stuff.
if (Meteor.isServer) {

  // Global API configuration
  var Api = new Restivus({
    useDefaultAuth: true,
    prettyJson: true
  });

  Api.addCollection(Meteor.users, {
    excludedEndpoints: ['getAll', 'put'],
    routeOptions: {
      authRequired: false
    },
    endpoints: {
      post: {
        authRequired: false
      },
      delete: {
        roleRequired: 'admin'
      }
    }
  });

  // Maps to: /api/getDat
  // Calling root/api/getDat will return the value of the data in DataColl
  Api.addRoute('getDat', {authRequired: false}, {
    get: function () {
          console.log(DataColl.findOne({},{sort: {timestamp: -1}} ).value);
      //return parseInt(DataColl.findOne({},{sort: {timestamp: -1}} ).value);
        return DataColl.findOne({},{sort: {timestamp: -1}} ).value;
    }
  });

  
  
  // Maps to: /api/postDat
  // Calling root/api/postData/xxx will give the the data in DataColl a value of xxx
   Api.addRoute('postDat/:message', {}, {

        post: {
            action: function(){

                var response = null;
                var message = this.urlParams.message;

                if(message){
				
							var valP = message; // 
							DataID = DataColl.findOne()._id;
							DataColl.update(DataID ,{$set: {value: valP} });
				
				
				
                    console.log("Message received: " + message);
                    return {status: "success", data: message};
                } else {
                    console.log("Message empty...");
                    return {status: "fail", message: "Post not found"};
                }


                return;
            }
        }
    })


  
  
  
  
  
  
}
	